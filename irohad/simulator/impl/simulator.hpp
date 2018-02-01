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

#ifndef IROHA_SIMULATOR_HPP
#define IROHA_SIMULATOR_HPP

#include <nonstd/optional.hpp>
#include "ametsuchi/block_query.hpp"
#include "ametsuchi/temporary_factory.hpp"
#include "model/model_crypto_provider.hpp"
#include "network/ordering_gate.hpp"
#include "simulator/block_creator.hpp"
#include "simulator/verified_proposal_creator.hpp"
#include "validation/stateful_validator.hpp"

#include "logger/logger.hpp"

namespace iroha {
  namespace simulator {

    class Simulator : public VerifiedProposalCreator, public BlockCreator {
     public:
      Simulator(
          std::shared_ptr<network::OrderingGate> ordering_gate,
          std::shared_ptr<validation::StatefulValidator> statefulValidator,
          std::shared_ptr<ametsuchi::TemporaryFactory> factory,
          std::shared_ptr<ametsuchi::BlockQuery> blockQuery,
          std::shared_ptr<model::ModelCryptoProvider> crypto_provider);

      Simulator(const Simulator &) = delete;
      Simulator &operator=(const Simulator &) = delete;

      void process_proposal(model::Proposal proposal) override;

      rxcpp::observable<model::Proposal> on_verified_proposal() override;

      void process_verified_proposal(model::Proposal proposal) override;

      rxcpp::observable<model::Block> on_block() override;

     private:
      // internal
      rxcpp::subjects::subject<model::Proposal> notifier_;
      rxcpp::subjects::subject<model::Block> block_notifier_;

      std::shared_ptr<validation::StatefulValidator> validator_;
      std::shared_ptr<ametsuchi::TemporaryFactory> ametsuchi_factory_;
      std::shared_ptr<ametsuchi::BlockQuery> block_queries_;
      std::shared_ptr<model::ModelCryptoProvider> crypto_provider_;

      logger::Logger log_;

      // last block
      nonstd::optional<model::Block> last_block;
    };
  }  // namespace simulator
}  // namespace iroha

#endif  // IROHA_SIMULATOR_HPP
