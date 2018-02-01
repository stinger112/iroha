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

#include "consensus/yac/storage/yac_block_storage.hpp"

#include <algorithm>
#include <utility>

#include "consensus/consensus_common.hpp"

using namespace logger;

namespace iroha {
  namespace consensus {
    namespace yac {

      // --------| Public API |--------

      YacBlockStorage::YacBlockStorage(YacHash hash, uint64_t peers_in_round)
          : hash_(std::move(hash)), peers_in_round_(peers_in_round) {
        log_ = log("YacBlockStorage");
      }

      nonstd::optional<Answer> YacBlockStorage::insert(VoteMessage msg) {
        if (validScheme(msg) and uniqueVote(msg)) {
          votes_.push_back(msg);

          log_->info("Vote ({}, {}) inserted",
                     msg.hash.proposal_hash,
                     msg.hash.block_hash);
          log_->info(
              "Votes in storage [{}/{}]", votes_.size(), peers_in_round_);
        }
        return getState();
      }

      nonstd::optional<Answer> YacBlockStorage::insert(
          std::vector<VoteMessage> votes) {
        std::for_each(votes.begin(), votes.end(), [this](auto vote) {
          this->insert(vote);
        });
        return getState();
      }

      auto YacBlockStorage::getVotes() -> decltype(votes_) {
        return votes_;
      }

      auto YacBlockStorage::getNumberOfVotes() -> decltype(votes_)::size_type {
        return votes_.size();
      }

      nonstd::optional<Answer> YacBlockStorage::getState() {
        auto supermajority = hasSupermajority(votes_.size(), peers_in_round_);
        if (supermajority) {
          return Answer(CommitMessage(votes_));
        }
        return nonstd::nullopt;
      }

      bool YacBlockStorage::isContains(const VoteMessage &msg) const {
        return std::count(votes_.begin(), votes_.end(), msg) != 0;
      }

      YacHash YacBlockStorage::getStorageHash() {
        return hash_;
      }

      // --------| private api |--------

      bool YacBlockStorage::uniqueVote(VoteMessage &msg) {
        // lookup take O(n) times
        return std::all_of(votes_.begin(), votes_.end(), [&msg](auto vote) {
          return vote != msg;
        });
      }

      bool YacBlockStorage::validScheme(VoteMessage &vote) {
        return getStorageHash() == vote.hash;
      }

    }  // namespace yac
  }    // namespace consensus
}  // namespace iroha
