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

#include <grpc++/grpc++.h>

#include "logger/logger.hpp"
#include "network/ordering_service.hpp"

#include "module/irohad/ametsuchi/ametsuchi_mocks.hpp"
#include "module/irohad/network/network_mocks.hpp"

#include "ordering/impl/ordering_gate_impl.hpp"
#include "ordering/impl/ordering_gate_transport_grpc.hpp"
#include "ordering/impl/ordering_service_impl.hpp"
#include "ordering/impl/ordering_service_transport_grpc.hpp"

using namespace iroha;
using namespace iroha::ordering;
using namespace iroha::model;
using namespace iroha::network;
using namespace iroha::ametsuchi;
using namespace std::chrono_literals;

using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::Return;

static logger::Logger log_ = logger::testLog("OrderingService");

class MockOrderingServiceTransport : public network::OrderingServiceTransport {
 public:
  void publishProposal(model::Proposal &&proposal,
                       const std::vector<std::string> &peers) override {
    publishProposal(proposal, peers);
  };

  void subscribe(std::shared_ptr<network::OrderingServiceNotification>
                     subscriber) override {
    subscriber_ = subscriber;
  }

  MOCK_METHOD2(publishProposal,
               void(const model::Proposal &proposal,
                    const std::vector<std::string> &peers));

  std::weak_ptr<network::OrderingServiceNotification> subscriber_;
};

class OrderingServiceTest : public ::testing::Test {
 public:
  OrderingServiceTest() {
    peer.address = address;
  }

  void SetUp() override {
    wsv = std::make_shared<MockPeerQuery>();
    fake_transport = std::make_shared<MockOrderingServiceTransport>();
  }

  std::shared_ptr<MockOrderingServiceTransport> fake_transport;
  std::condition_variable cv;
  std::mutex m;
  std::string address{"0.0.0.0:50051"};
  model::Peer peer;
  std::shared_ptr<MockPeerQuery> wsv;
};

TEST_F(OrderingServiceTest, SimpleTest) {
  // Direct publishProposal call, used for basic case test and for debug
  // simplicity

  const size_t max_proposal = 5;
  const size_t commit_delay = 1000;

  auto ordering_service = std::make_shared<OrderingServiceImpl>(
      wsv, max_proposal, commit_delay, fake_transport);
  fake_transport->subscribe(ordering_service);

  EXPECT_CALL(*fake_transport, publishProposal(_, _)).Times(1);

  fake_transport->publishProposal(model::Proposal({}), {});
}

TEST_F(OrderingServiceTest, ValidWhenProposalSizeStrategy) {
  const size_t max_proposal = 5;
  const size_t commit_delay = 1000;

  auto ordering_service = std::make_shared<OrderingServiceImpl>(
      wsv, max_proposal, commit_delay, fake_transport);
  fake_transport->subscribe(ordering_service);

  // Init => proposal size 5 => 2 proposals after 10 transactions
  size_t call_count = 0;
  EXPECT_CALL(*fake_transport, publishProposal(_, _))
      .Times(2)
      .WillRepeatedly(InvokeWithoutArgs([&] {
        ++call_count;
        cv.notify_one();
      }));

  EXPECT_CALL(*wsv, getLedgerPeers())
      .WillRepeatedly(Return(std::vector<Peer>{peer}));

  for (size_t i = 0; i < 10; ++i) {
    ordering_service->onTransaction(model::Transaction());
  }

  std::unique_lock<std::mutex> lock(m);
  cv.wait_for(lock, 10s, [&] { return call_count == 2; });
}

TEST_F(OrderingServiceTest, ValidWhenTimerStrategy) {
  // Init => proposal timer 400 ms => 10 tx by 50 ms => 2 proposals in 1 second

  EXPECT_CALL(*wsv, getLedgerPeers())
      .WillRepeatedly(Return(std::vector<Peer>{peer}));

  const size_t max_proposal = 100;
  const size_t commit_delay = 400;

  auto ordering_service = std::make_shared<OrderingServiceImpl>(
      wsv, max_proposal, commit_delay, fake_transport);
  fake_transport->subscribe(ordering_service);

  EXPECT_CALL(*fake_transport, publishProposal(_, _))
      .Times(2)
      .WillRepeatedly(InvokeWithoutArgs([&] {
        log_->info("Proposal send to grpc");
        cv.notify_one();
      }));

  for (size_t i = 0; i < 8; ++i) {
    ordering_service->onTransaction(model::Transaction());
  }

  std::unique_lock<std::mutex> lk(m);
  cv.wait_for(lk, 10s);

  ordering_service->onTransaction(model::Transaction());
  ordering_service->onTransaction(model::Transaction());
  cv.wait_for(lk, 10s);
}
