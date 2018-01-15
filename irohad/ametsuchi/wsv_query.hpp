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

#ifndef IROHA_WSV_QUERY_HPP
#define IROHA_WSV_QUERY_HPP

#include <common/types.hpp>
#include <model/account.hpp>
#include <model/account_asset.hpp>
#include <model/asset.hpp>
#include <model/peer.hpp>
#include <nonstd/optional.hpp>
#include <boost/variant.hpp>
#include <string>
#include <vector>
#include "model/domain.hpp"

namespace outcome {

  template <typename E>
  struct Error {
    Error(E &&e): e(std::forward<E>(e)) {};
    E e;
  };

  template <typename V>
  struct Value {
    Value(V &&v): v(std::forward<V>(v)) {};
    V v;
  };

  template <typename T, typename E>
  using result = boost::variant<Value<T>, Error<E>>;

  template <typename ValueHandler, typename ErrorHandler>
  class ResultVisitor : public ValueHandler, public ErrorHandler {
   public:
    ResultVisitor(ValueHandler&& value_handler, ErrorHandler&& error_handler):
        vhandler_{std::forward<ValueHandler>(value_handler)},
        ehandler_{std::forward<ErrorHandler>(error_handler)} {};

    using ValueHandler::operator();
    using ErrorHandler::operator();
   private:
    ValueHandler vhandler_;
    ErrorHandler ehandler_;
  };

  template <typename T, typename E, typename VHandle, typename EHandle>
  auto match(const result<T, E> &r, VHandle &&value_f, EHandle &&error_f) {
    ResultVisitor<VHandle, EHandle> matcher(value_f, error_f);
    return boost::apply_visitor(matcher, r);
  };
}

namespace iroha {
  namespace ametsuchi {
    /**
     *  Public interface for world state view queries
     */
    class WsvQuery {
     public:
      virtual ~WsvQuery() = default;

      /**
       * Check if permitee has permission on account
       * @param permitee_account_id
       * @param account_id
       * @param permission_id
       * @return true if has permission, false otherwise
       */
      virtual bool hasAccountGrantablePermission(
          const std::string &permitee_account_id,
          const std::string &account_id,
          const std::string &permission_id) = 0;

      /**
       * Get iroha domain
       * @param domain_id - id in the system
       * @return Domain if exist, nullopt otherwise
       */
      virtual outcome::result<model::Domain, std::error_code> getDomain(
          const std::string &domain_id) = 0;

      /**
       * Get account's roles
       * @param account_id
       * @return
       */
      virtual outcome::result<std::vector<std::string>, std::error_code> getAccountRoles(
          const std::string &account_id) = 0;
      /**
       * Get all permissions of a role
       * @param role_name
       * @return
       */
      virtual outcome::result<std::vector<std::string>, std::error_code> getRolePermissions(
          const std::string &role_name) = 0;

      /**
       * @return All roles currently in the system
       */
      virtual outcome::result<std::vector<std::string>, std::error_code> getRoles() = 0;
      /**
       * Get account by user account_id
       * @param account_id
       * @return
       */
      virtual outcome::result<model::Account, std::error_code> getAccount(
          const std::string &account_id) = 0;

      /**
       * Get accounts information from its key-value storage
       * @param account_id
       * @param creator_account_id
       * @param detail
       * @return
       */
      virtual outcome::result<std::string, std::error_code> getAccountDetail(
          const std::string &account_id,
          const std::string &creator_account_id,
          const std::string &detail) = 0;

      /**
       * Get signatories of account by user account_id
       * @param account_id
       * @return
       */
      virtual outcome::result<std::vector<pubkey_t>, std::error_code> getSignatories(
          const std::string &account_id) = 0;

      /**
       * Get asset by its name
       * @param asset_id
       * @return
       */
      virtual outcome::result<model::Asset, std::error_code> getAsset(
          const std::string &asset_id) = 0;

      /**
       *
       * @param account_id
       * @param asset_id
       * @return
       */
      virtual outcome::result<model::AccountAsset, std::error_code> getAccountAsset(
          const std::string &account_id, const std::string &asset_id) = 0;

      /**
       *
       * @return
       */
      virtual outcome::result<std::vector<model::Peer>, std::error_code> getPeers() = 0;
    };

  }  // namespace ametsuchi
}  // namespace iroha

#endif  // IROHA_WSV_QUERY_HPP
