/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <grpc++/grpc++.h>

#include "backend/protobuf/common_objects/peer.hpp"
#include "backend/protobuf/proto_proposal_factory.hpp"
#include "builders/protobuf/common_objects/proto_peer_builder.hpp"
#include "framework/batch_helper.hpp"
#include "interfaces/iroha_internal/transaction_batch.hpp"
#include "logger/logger.hpp"
#include "module/irohad/ametsuchi/ametsuchi_mocks.hpp"
#include "module/irohad/network/network_mocks.hpp"
#include "module/irohad/ordering/mock_ordering_service_persistent_state.hpp"
#include "module/shared_model/builders/protobuf/test_proposal_builder.hpp"
#include "ordering/impl/ordering_service_impl.hpp"
#include "ordering/impl/ordering_service_transport_grpc.hpp"

using namespace iroha;
using namespace iroha::ordering;
using namespace iroha::network;
using namespace iroha::ametsuchi;
using namespace std::chrono_literals;

using ::testing::_;
using ::testing::AtLeast;
using ::testing::DoAll;
using ::testing::Invoke;
using ::testing::InvokeWithoutArgs;
using ::testing::Return;

class MockOrderingServiceTransport : public network::OrderingServiceTransport {
 public:
  void subscribe(std::shared_ptr<network::OrderingServiceNotification>
                     subscriber) override {
    subscriber_ = subscriber;
  }

  void publishProposal(
      std::unique_ptr<shared_model::interface::Proposal> proposal,
      const std::vector<std::string> &peers) override {
    return publishProposalProxy(proposal.get(), peers);
  }

  MOCK_METHOD2(publishProposalProxy,
               void(shared_model::interface::Proposal *proposal,
                    const std::vector<std::string> &peers));

  std::weak_ptr<network::OrderingServiceNotification> subscriber_;
};

class OrderingServiceTest : public ::testing::Test {
 public:
  OrderingServiceTest() {
    peer = clone(shared_model::proto::PeerBuilder()
                     .address(address)
                     .pubkey(shared_model::interface::types::PubkeyType(
                         std::string(32, '0')))
                     .build());
  }

  void SetUp() override {
    wsv = std::make_shared<MockPeerQuery>();
    fake_transport = std::make_shared<MockOrderingServiceTransport>();
    fake_persistent_state =
        std::make_shared<MockOrderingServicePersistentState>();
    factory = std::make_unique<shared_model::proto::ProtoProposalFactory<
        shared_model::validation::AlwaysValidValidator>>();
  }

  auto initOs(size_t max_proposal) {
    return std::make_shared<OrderingServiceImpl>(
        wsv,
        max_proposal,
        proposal_timeout.get_observable(),
        fake_transport,
        fake_persistent_state,
        std::move(factory),
        false);
  }

  void makeProposalTimeout() {
    proposal_timeout.get_subscriber().on_next(0);
  }

  std::shared_ptr<MockOrderingServiceTransport> fake_transport;
  std::shared_ptr<MockOrderingServicePersistentState> fake_persistent_state;
  std::condition_variable cv;
  std::mutex m;
  std::string address{"0.0.0.0:50051"};
  std::shared_ptr<shared_model::interface::Peer> peer;
  std::shared_ptr<MockPeerQuery> wsv;
  std::unique_ptr<shared_model::interface::ProposalFactory> factory;
  rxcpp::subjects::subject<OrderingServiceImpl::TimeoutType> proposal_timeout;
};

/**
 * @given OrderingService and MockOrderingServiceTransport
 * @when publishProposal is called at transport
 * @then publishProposalProxy is called
 */
TEST_F(OrderingServiceTest, SimpleTest) {
  const size_t max_proposal = 5;

  EXPECT_CALL(*fake_persistent_state, loadProposalHeight())
      .Times(1)
      .WillOnce(Return(boost::optional<size_t>(2)));
  EXPECT_CALL(*fake_transport, publishProposalProxy(_, _)).Times(1);

  auto ordering_service = initOs(max_proposal);
  fake_transport->subscribe(ordering_service);

  fake_transport->publishProposal(
      std::make_unique<shared_model::proto::Proposal>(
          TestProposalBuilder()
              .height(1)
              .createdTime(iroha::time::now())
              .build()),
      {});
}

/**
 * @given OrderingService with max_proposal==5 and only self peer
 *        and MockOrderingServiceTransport
 *        and MockOrderingServicePersistentState
 * @when OrderingService::onTransaction called 10 times
 * @then publishProposalProxy called twice
 *       and proposal height was loaded once and saved twice
 */
TEST_F(OrderingServiceTest, ValidWhenProposalSizeStrategy) {
  const size_t max_proposal = 5;
  const size_t tx_num = 10;

  EXPECT_CALL(*fake_persistent_state, saveProposalHeight(_))
      .Times(2)
      .WillRepeatedly(Return(true));
  EXPECT_CALL(*fake_persistent_state, loadProposalHeight())
      .Times(1)
      .WillOnce(Return(boost::optional<size_t>(2)));
  EXPECT_CALL(*fake_transport, publishProposalProxy(_, _))
      .Times(tx_num / max_proposal);
  EXPECT_CALL(*wsv, getLedgerPeers())
      .WillRepeatedly(Return(std::vector<decltype(peer)>{peer}));

  auto ordering_service = initOs(max_proposal);
  fake_transport->subscribe(ordering_service);

  for (size_t i = 0; i < tx_num; ++i) {
    ordering_service->onBatch(framework::batch::createValidBatch(1));
  }
}

/**
 * @given OrderingService with big enough max_proposal and only self peer
 *        and MockOrderingServiceTransport
 *        and MockOrderingServicePersistentState
 * @when OrderingService::onTransaction called 8 times
 *       and after triggered timeout
 *       and then repeat with 2 onTransaction calls
 * @then publishProposalProxy called twice
 *       and proposal height was loaded once and saved twice
 */
TEST_F(OrderingServiceTest, ValidWhenTimerStrategy) {
  const size_t max_proposal = 100;

  EXPECT_CALL(*fake_persistent_state, saveProposalHeight(_))
      .Times(2)
      .WillRepeatedly(Return(true));
  EXPECT_CALL(*fake_persistent_state, loadProposalHeight())
      .Times(1)
      .WillOnce(Return(boost::optional<size_t>(2)));
  EXPECT_CALL(*wsv, getLedgerPeers())
      .WillRepeatedly(Return(std::vector<decltype(peer)>{peer}));
  EXPECT_CALL(*fake_transport, publishProposalProxy(_, _)).Times(2);

  auto ordering_service = initOs(max_proposal);
  fake_transport->subscribe(ordering_service);

  for (size_t i = 0; i < 8; ++i) {
    ordering_service->onBatch(framework::batch::createValidBatch(1));
  }
  makeProposalTimeout();

  ordering_service->onBatch(framework::batch::createValidBatch(1));
  ordering_service->onBatch(framework::batch::createValidBatch(1));

  makeProposalTimeout();
}

/**
 * @given Ordering service and the persistent state that cannot save
 *        proposals
 * @when onTransaction is called
 * @then no published proposal
 */
TEST_F(OrderingServiceTest, BrokenPersistentState) {
  const size_t max_proposal = 1;

  EXPECT_CALL(*fake_persistent_state, loadProposalHeight())
      .Times(1)
      .WillOnce(Return(boost::optional<size_t>(1)));
  EXPECT_CALL(*fake_persistent_state, saveProposalHeight(2))
      .Times(1)
      .WillRepeatedly(Return(false));

  auto ordering_service = initOs(max_proposal);
  ordering_service->onBatch(framework::batch::createValidBatch(1));

  makeProposalTimeout();
}

/**
 * @given Ordering service up and running
 * @when Send 1000 transactions from each of 2 threads
 * @then Ordering service should not crash
 */
TEST_F(OrderingServiceTest, ConcurrentGenerateProposal) {
  const auto max_proposal = 1;
  EXPECT_CALL(*fake_persistent_state, loadProposalHeight())
      .Times(1)
      .WillOnce(Return(boost::optional<size_t>(1)));
  EXPECT_CALL(*fake_persistent_state, saveProposalHeight(_))
      .WillRepeatedly(Return(false));

  auto ordering_service = initOs(max_proposal);

  auto on_tx = [&]() {
    for (int i = 0; i < 1000; ++i) {
      ordering_service->onBatch(framework::batch::createValidBatch(1));
    }
  };

  const auto num_threads = 2;

  std::vector<std::thread> threads;
  for (int i = 0; i < num_threads; ++i) {
    threads.emplace_back(std::thread(on_tx));
  }

  for (int i = 0; i < num_threads; ++i) {
    threads.at(i).join();
  }
  makeProposalTimeout();
}

/**
 * @given Ordering service up and running
 * @when Send 1000 transactions from a separate thread and perform 5 second
 * delay during generateProposal() so destructor of OrderingServiceImpl is
 * called before generateProposal() finished
 * @then Ordering service should not crash and publishProposal() should not be
 * called after destructor call
 */
TEST_F(OrderingServiceTest, GenerateProposalDestructor) {
  const auto max_proposal = 600;
  const auto commit_delay = 5s;
  EXPECT_CALL(*fake_persistent_state, loadProposalHeight())
      .Times(1)
      .WillOnce(Return(boost::optional<size_t>(1)));
  EXPECT_CALL(*fake_persistent_state, saveProposalHeight(_))
      .WillRepeatedly(InvokeWithoutArgs([] {
        std::this_thread::sleep_for(5s);
        return true;
      }));
  EXPECT_CALL(*wsv, getLedgerPeers())
      .WillRepeatedly(Return(std::vector<decltype(peer)>{peer}));

  {
    EXPECT_CALL(*fake_transport, publishProposalProxy(_, _)).Times(AtLeast(1));
    OrderingServiceImpl ordering_service(
        wsv,
        max_proposal,
        rxcpp::observable<>::interval(commit_delay,
                                      rxcpp::observe_on_new_thread()),
        fake_transport,
        fake_persistent_state,
        std::move(factory),
        true);

    auto on_tx = [&]() {
      // create max_proposal+1 txs, so that publish proposal is invoked at least
      // once (concurrency!)
      for (int i = 0; i < max_proposal + 1; ++i) {
        ordering_service.onBatch(framework::batch::createValidBatch(1));
      }
    };

    std::thread thread(on_tx);
    thread.join();
  }
  EXPECT_CALL(*fake_transport, publishProposalProxy(_, _)).Times(0);
}

/**
 * Check that batches are processed by ordering service
 * @given ordering service up and running
 * @when feeding the ordering service two batches, such that number of
 * transactions in both summed is greater than maximum number of transactions
 * inside proposal
 * @then proposal will still contain number of transactions, equal to sum of the
 * batches
 */
TEST_F(OrderingServiceTest, BatchesProceed) {
  const auto max_proposal = 12;
  const auto first_batch_size = 10;
  const auto second_batch_size = 5;

  auto batch_one = framework::batch::createValidBatch(first_batch_size);
  auto batch_two = framework::batch::createValidBatch(second_batch_size);

  EXPECT_CALL(*fake_persistent_state, saveProposalHeight(_))
      .Times(1)
      .WillRepeatedly(Return(true));
  EXPECT_CALL(*fake_persistent_state, loadProposalHeight())
      .Times(1)
      .WillOnce(Return(
          boost::optional<size_t>(first_batch_size + second_batch_size)));
  EXPECT_CALL(*fake_transport, publishProposalProxy(_, _)).Times(1);
  EXPECT_CALL(*wsv, getLedgerPeers())
      .WillRepeatedly(Return(std::vector<decltype(peer)>{peer}));

  auto ordering_service = initOs(max_proposal);
  fake_transport->subscribe(ordering_service);

  ordering_service->onBatch(std::move(batch_one));
  ordering_service->onBatch(std::move(batch_two));
}
