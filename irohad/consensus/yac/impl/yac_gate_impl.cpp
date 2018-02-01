/**
 * Copyright Soramitsu Co., Ltd. 2017 All Rights Reserved.
 * http://soramitsu.co.jp
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *        http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "consensus/yac/impl/yac_gate_impl.hpp"

#include "consensus/yac/storage/yac_common.hpp"

namespace iroha {
  namespace consensus {
    namespace yac {

      YacGateImpl::YacGateImpl(
          std::shared_ptr<HashGate> hash_gate,
          std::shared_ptr<YacPeerOrderer> orderer,
          std::shared_ptr<YacHashProvider> hash_provider,
          std::shared_ptr<simulator::BlockCreator> block_creator,
          std::shared_ptr<network::BlockLoader> block_loader,
          uint64_t delay)
          : hash_gate_(std::move(hash_gate)),
            orderer_(std::move(orderer)),
            hash_provider_(std::move(hash_provider)),
            block_creator_(std::move(block_creator)),
            block_loader_(std::move(block_loader)),
            delay_(delay) {
        log_ = logger::log("YacGate");
        block_creator_->on_block().subscribe(
            [this](auto block) { this->vote(block); });
      }

      void YacGateImpl::vote(model::Block block) {
        auto hash = hash_provider_->makeHash(block);
        log_->info(
            "vote for block ({}, {})", hash.proposal_hash, hash.block_hash);
        auto order = orderer_->getOrdering(hash);
        if (not order.has_value()) {
          log_->error("ordering doesn't provide peers => pass round");
          return;
        }
        current_block_ = std::make_pair(hash, block);
        hash_gate_->vote(hash, order.value());
      }

      rxcpp::observable<model::Block> YacGateImpl::on_commit() {
        return hash_gate_->on_commit().flat_map([this](auto commit_message) {
          // map commit to block if it is present or loaded from other peer
          return rxcpp::observable<>::create<model::Block>(
              [this, commit_message](auto subscriber) {
                const auto hash = getHash(commit_message.votes);
                if (not hash.has_value()) {
                  log_->info("Invalid commit message, hashes are different");
                  subscriber.on_completed();
                  return;
                }
                // if node has voted for the committed block
                if (hash == current_block_.first) {
                  // append signatures of other nodes
                  this->copySignatures(commit_message);
                  log_->info("consensus: commit top block: height {}, hash {}",
                             current_block_.second.height,
                             current_block_.second.hash.to_hexstring());
                  subscriber.on_next(current_block_.second);
                  subscriber.on_completed();
                  return;
                }
                // node has voted for another block - load committed block
                const auto model_hash =
                    hash_provider_->toModelHash(hash.value());
                // iterate over peers who voted for the committed block
                rxcpp::observable<>::iterate(commit_message.votes)
                    // allow other peers to apply commit
                    .delay(std::chrono::milliseconds(delay_))
                    .flat_map([this, model_hash](auto vote) {
                      // map vote to block if it can be loaded
                      return rxcpp::observable<>::create<model::Block>(
                          [this, model_hash, vote](auto subscriber) {
                            auto block = block_loader_->retrieveBlock(
                                vote.signature.pubkey, model_hash);
                            // if load is successful
                            if (block.has_value()) {
                              subscriber.on_next(block.value());
                            }
                            subscriber.on_completed();
                          });
                    })
                    // need only the first
                    .first()
                    .subscribe(
                        // if load is successful from at least one node
                        [subscriber](auto block) {
                          subscriber.on_next(block);
                          subscriber.on_completed();
                        },
                        // if load has failed, no peers provided the block
                        [this, subscriber](std::exception_ptr) {
                          log_->error("Cannot load committed block");
                          subscriber.on_completed();
                        });
              });
        });
      }

      void YacGateImpl::copySignatures(const CommitMessage &commit) {
        current_block_.second.sigs.clear();
        for (const auto &vote : commit.votes) {
          current_block_.second.sigs.push_back(vote.hash.block_signature);
        }
      }
    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha
