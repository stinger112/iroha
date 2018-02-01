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

#include "main/impl/ordering_init.hpp"
#include "ordering/impl/ordering_service_transport_grpc.hpp"

namespace iroha {
  namespace network {
    auto OrderingInit::createGate(
        std::shared_ptr<OrderingGateTransport> transport) {
      auto gate = std::make_shared<ordering::OrderingGateImpl>(transport);
      transport->subscribe(gate);
      return gate;
    }

    auto OrderingInit::createService(
        std::shared_ptr<ametsuchi::PeerQuery> wsv,
        size_t max_size,
        std::chrono::milliseconds delay_milliseconds,
        std::shared_ptr<network::OrderingServiceTransport> transport) {
      return std::make_shared<ordering::OrderingServiceImpl>(
          wsv, max_size, delay_milliseconds.count(), transport);
    }

    std::shared_ptr<ordering::OrderingGateImpl> OrderingInit::initOrderingGate(
        std::shared_ptr<ametsuchi::PeerQuery> wsv,
        size_t max_size,
        std::chrono::milliseconds delay_milliseconds) {
      auto network_address = wsv->getLedgerPeers().value().front().address;
      ordering_gate_transport =
          std::make_shared<iroha::ordering::OrderingGateTransportGrpc>(
              network_address);

      ordering_service_transport =
          std::make_shared<ordering::OrderingServiceTransportGrpc>();
      ordering_service = createService(
          wsv, max_size, delay_milliseconds, ordering_service_transport);
      ordering_service_transport->subscribe(ordering_service);
      ordering_gate = createGate(ordering_gate_transport);
      return ordering_gate;
    }
  }  // namespace network
}  // namespace iroha
