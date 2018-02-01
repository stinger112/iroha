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

#include "module/irohad/ametsuchi/ametsuchi_mocks.hpp"
#include "module/irohad/network/network_mocks.hpp"
#include "module/irohad/validation/validation_mocks.hpp"

#include "framework/test_subscriber.hpp"
#include "synchronizer/impl/synchronizer_impl.hpp"
#include "validation/chain_validator.hpp"

using namespace iroha;
using namespace iroha::model;
using namespace iroha::ametsuchi;
using namespace iroha::synchronizer;
using namespace iroha::validation;
using namespace iroha::network;
using namespace framework::test_subscriber;

using ::testing::_;
using ::testing::DefaultValue;
using ::testing::Return;

class SynchronizerTest : public ::testing::Test {
 public:
  void SetUp() override {
    chain_validator = std::make_shared<MockChainValidator>();
    mutable_factory = std::make_shared<MockMutableFactory>();
    block_loader = std::make_shared<MockBlockLoader>();
    consensus_gate = std::make_shared<MockConsensusGate>();
  }

  void init() {
    synchronizer = std::make_shared<SynchronizerImpl>(
        consensus_gate, chain_validator, mutable_factory, block_loader);
  }

  std::shared_ptr<MockChainValidator> chain_validator;
  std::shared_ptr<MockMutableFactory> mutable_factory;
  std::shared_ptr<MockBlockLoader> block_loader;
  std::shared_ptr<MockConsensusGate> consensus_gate;

  std::shared_ptr<SynchronizerImpl> synchronizer;
};

TEST_F(SynchronizerTest, ValidWhenInitialized) {
  // synchronizer constructor => on_commit subscription called
  EXPECT_CALL(*consensus_gate, on_commit())
      .WillOnce(Return(rxcpp::observable<>::empty<Block>()));

  init();
}

TEST_F(SynchronizerTest, ValidWhenSingleCommitSynchronized) {
  // commit from consensus => chain validation passed => commit successful
  Block test_block;
  test_block.height = 5;

  DefaultValue<std::unique_ptr<MutableStorage>>::SetFactory(
      &createMockMutableStorage);
  EXPECT_CALL(*mutable_factory, createMutableStorage()).Times(1);

  EXPECT_CALL(*mutable_factory, commit_(_)).Times(1);

  EXPECT_CALL(*chain_validator, validateBlock(test_block, _))
      .WillOnce(Return(true));

  EXPECT_CALL(*block_loader, retrieveBlocks(_)).Times(0);

  EXPECT_CALL(*consensus_gate, on_commit())
      .WillOnce(Return(rxcpp::observable<>::empty<Block>()));

  init();

  auto wrapper =
      make_test_subscriber<CallExact>(synchronizer->on_commit_chain(), 1);
  wrapper.subscribe([&test_block](auto commit) {
    auto block_wrapper = make_test_subscriber<CallExact>(commit, 1);
    block_wrapper.subscribe([&test_block](auto block) {
      // Check commit block
      ASSERT_EQ(block.height, test_block.height);
    });
    ASSERT_TRUE(block_wrapper.validate());
  });

  synchronizer->process_commit(test_block);

  ASSERT_TRUE(wrapper.validate());
}

TEST_F(SynchronizerTest, ValidWhenBadStorage) {
  // commit from consensus => storage not created => no commit
  Block test_block;

  DefaultValue<std::unique_ptr<MutableStorage>>::Clear();
  EXPECT_CALL(*mutable_factory, createMutableStorage()).Times(1);

  EXPECT_CALL(*mutable_factory, commit_(_)).Times(0);

  EXPECT_CALL(*chain_validator, validateBlock(test_block, _)).Times(0);

  EXPECT_CALL(*block_loader, retrieveBlocks(_)).Times(0);

  EXPECT_CALL(*consensus_gate, on_commit())
      .WillOnce(Return(rxcpp::observable<>::empty<Block>()));

  init();

  auto wrapper =
      make_test_subscriber<CallExact>(synchronizer->on_commit_chain(), 0);
  wrapper.subscribe();

  synchronizer->process_commit(test_block);

  ASSERT_TRUE(wrapper.validate());
}

TEST_F(SynchronizerTest, ValidWhenBlockValidationFailure) {
  // commit from consensus => chain validation failed => commit successful
  Block test_block;
  test_block.height = 5;
  test_block.sigs.emplace_back();

  DefaultValue<std::unique_ptr<MutableStorage>>::SetFactory(
      &createMockMutableStorage);
  EXPECT_CALL(*mutable_factory, createMutableStorage()).Times(2);

  EXPECT_CALL(*mutable_factory, commit_(_)).Times(1);

  EXPECT_CALL(*chain_validator, validateBlock(test_block, _))
      .WillOnce(Return(false));
  EXPECT_CALL(*chain_validator, validateChain(_, _)).WillOnce(Return(true));

  EXPECT_CALL(*block_loader, retrieveBlocks(_))
      .WillOnce(Return(rxcpp::observable<>::just(test_block)));

  EXPECT_CALL(*consensus_gate, on_commit())
      .WillOnce(Return(rxcpp::observable<>::empty<Block>()));

  init();

  auto wrapper =
      make_test_subscriber<CallExact>(synchronizer->on_commit_chain(), 1);
  wrapper.subscribe([&test_block](auto commit) {
    auto block_wrapper = make_test_subscriber<CallExact>(commit, 1);
    block_wrapper.subscribe([&test_block](auto block) {
      // Check commit block
      ASSERT_EQ(block.height, test_block.height);
    });
    ASSERT_TRUE(block_wrapper.validate());
  });

  synchronizer->process_commit(test_block);

  ASSERT_TRUE(wrapper.validate());
}
