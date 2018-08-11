/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include <gtest/gtest.h>

#include "builders/protobuf/queries.hpp"
#include "cryptography/crypto_provider/crypto_defaults.hpp"
#include "framework/integration_framework/integration_test_framework.hpp"
#include "framework/specified_visitor.hpp"
#include "module/shared_model/builders/protobuf/test_query_builder.hpp"

template <typename BaseType>
auto makeQuery() {
  return BaseType()
      .createdTime(iroha::time::now())
      .creatorAccountId("admin@test")
      .queryCounter(1)
      .getAccount("admin@test")
      .build();
}

template <typename Query>
auto createInvalidQuery(Query query,
                        const shared_model::crypto::Keypair &keypair) {
  query.addSignature(shared_model::crypto::Signed(std::string(32, 'a')),
                     keypair.publicKey());
  return query;
}

/**
 * @given itf instance
 * @when  pass query with invalid signature
 * @then  assure that query with invalid signature is failed with stateless
 * error
 */
TEST(QueryTest, FailedQueryTest) {
  const auto key_pair =
      shared_model::crypto::DefaultCryptoAlgorithmType::generateKeypair();

  auto query_with_broken_signature =
      createInvalidQuery(makeQuery<TestQueryBuilder>(), key_pair);
  auto stateless_invalid_query_response = [](auto &status) {
    auto &resp =
        boost::apply_visitor(framework::SpecifiedVisitor<
                                 shared_model::interface::ErrorQueryResponse>(),
                             status.get());
    boost::apply_visitor(
        framework::SpecifiedVisitor<
            shared_model::interface::StatelessFailedErrorResponse>(),
        resp.get());
  };

  integration_framework::IntegrationTestFramework itf(1);

  itf.setInitialState(key_pair).sendQuery(query_with_broken_signature,
                                          stateless_invalid_query_response);
}

/**
 * @given itf instance
 * @when  pass block query with invalid signature
 * @then  assure that query with invalid signature is failed with stateless
 * error
 */
TEST(QueryTest, FailedBlockQueryTest) {
  // TODO: 01/08/2018 @muratovv Implement test since IR-1569 will be completed
}
