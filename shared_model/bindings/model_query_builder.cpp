/**
 * Copyright Soramitsu Co., Ltd. All Rights Reserved.
 * SPDX-License-Identifier: Apache-2.0
 */

#include "bindings/model_query_builder.hpp"

#ifdef EMSCRIPTEN
#include "utils/emscripten_helpers.hpp"
#endif

namespace shared_model {
  namespace bindings {
    ModelQueryBuilder::ModelQueryBuilder() {
      *this = creatorAccountId("").createdTime(0).queryCounter(0);
    }

    ModelQueryBuilder ModelQueryBuilder::createdTime(
        interface::types::TimestampType created_time) {
      return ModelQueryBuilder(builder_.createdTime(created_time));
    }

    ModelQueryBuilder ModelQueryBuilder::creatorAccountId(
        const interface::types::AccountIdType &creator_account_id) {
      return ModelQueryBuilder(builder_.creatorAccountId(creator_account_id));
    }

    ModelQueryBuilder ModelQueryBuilder::queryCounter(
        interface::types::CounterType query_counter) {
      return ModelQueryBuilder(builder_.queryCounter(query_counter));
    }

    ModelQueryBuilder ModelQueryBuilder::getAccount(
        const interface::types::AccountIdType &account_id) {
      return ModelQueryBuilder(builder_.getAccount(account_id));
    }

    ModelQueryBuilder ModelQueryBuilder::getSignatories(
        const interface::types::AccountIdType &account_id) {
      return ModelQueryBuilder(builder_.getSignatories(account_id));
    }

    ModelQueryBuilder ModelQueryBuilder::getAccountTransactions(
        const interface::types::AccountIdType &account_id) {
      return ModelQueryBuilder(builder_.getAccountTransactions(account_id));
    }

    ModelQueryBuilder ModelQueryBuilder::getAccountAssetTransactions(
        const interface::types::AccountIdType &account_id,
        const interface::types::AssetIdType &asset_id) {
      return ModelQueryBuilder(
          builder_.getAccountAssetTransactions(account_id, asset_id));
    }

    ModelQueryBuilder ModelQueryBuilder::getAccountAssets(
        const interface::types::AccountIdType &account_id) {
      return ModelQueryBuilder(builder_.getAccountAssets(account_id));
    }

    ModelQueryBuilder ModelQueryBuilder::getRoles() {
      return ModelQueryBuilder(builder_.getRoles());
    }

    ModelQueryBuilder ModelQueryBuilder::getAssetInfo(
        const interface::types::AssetIdType &asset_id) {
      return ModelQueryBuilder(builder_.getAssetInfo(asset_id));
    }

    ModelQueryBuilder ModelQueryBuilder::getRolePermissions(
        const interface::types::RoleIdType &role_id) {
      return ModelQueryBuilder(builder_.getRolePermissions(role_id));
    }

    ModelQueryBuilder ModelQueryBuilder::getTransactions(
        const std::vector<crypto::Hash> &hashes) {
      return ModelQueryBuilder(builder_.getTransactions(hashes));
    }

    ModelQueryBuilder ModelQueryBuilder::getAccountDetail(
        const interface::types::AccountIdType &account_id,
        const interface::types::AccountDetailKeyType &key,
        const interface::types::AccountIdType &writer) {
      return ModelQueryBuilder(
          builder_.getAccountDetail(account_id, key, writer));
    }

    ModelQueryBuilder ModelQueryBuilder::getPendingTransactions() {
      return ModelQueryBuilder(builder_.getPendingTransactions());
    }

    proto::UnsignedWrapper<proto::Query> ModelQueryBuilder::build() {
      return builder_.build();
    }

  #ifdef EMSCRIPTEN
    ModelQueryBuilder ModelQueryBuilder::createdTime(
        const emscripten::val &created_time) {
      return this->createdTime(convert_from_js_to_uint64(created_time));
    }

    ModelQueryBuilder ModelQueryBuilder::queryCounter(
        const emscripten::val &query_counter) {
      return this->queryCounter(convert_from_js_to_uint64(query_counter));
    }
  #endif
  }  // namespace bindings
}  // namespace shared_model
