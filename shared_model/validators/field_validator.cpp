/**
 * Copyright Soramitsu Co., Ltd. 2018 All Rights Reserved.
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

#include "validators/field_validator.hpp"
#include <boost/format.hpp>

#include "validator/address_validator.hpp"

namespace shared_model {
  namespace validation {

    FieldValidator::FieldValidator(time_t future_gap)
        : account_id_pattern_(R"([a-z]{1,9}\@[a-z]{1,9})"),
          asset_id_pattern_(R"([a-z]{1,9}\#[a-z]{1,9})"),
          name_pattern_(R"([a-z]{1,9})"),
          detail_key_pattern_(R"([A-Za-z0-9_]{1,})"),
          future_gap_(future_gap) {}

    void FieldValidator::validateAccountId(
        ReasonsGroupType &reason,
        const interface::types::AccountIdType &account_id) const {
      if (not std::regex_match(account_id, account_id_pattern_)) {
        auto message =
            (boost::format("Wrongly formed account_id, passed value: '%s'")
             % account_id)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validateAssetId(
        ReasonsGroupType &reason,
        const interface::types::AssetIdType &asset_id) const {
      if (not std::regex_match(asset_id, asset_id_pattern_)) {
        auto message =
            (boost::format("Wrongly formed asset_id, passed value: '%s'")
             % asset_id)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validatePeer(ReasonsGroupType &reason,
                                      const interface::Peer &peer) const {
      validatePeerAddress(reason, peer.address());
      validatePubkey(reason, peer.pubkey());
    }

    void FieldValidator::validateAmount(ReasonsGroupType &reason,
                                        const interface::Amount &amount) const {
      if (amount.intValue() <= 0) {
        auto message =
            (boost::format("Amount must be greater than 0, passed value: %d")
             % amount.intValue())
                .str();
        reason.second.push_back(message);
      }
    }

    void FieldValidator::validatePubkey(
        ReasonsGroupType &reason,
        const interface::types::PubkeyType &pubkey) const {
      if (pubkey.blob().size() != key_size) {
        auto message =
            (boost::format("Public key has wrong size, passed size: %d")
             % pubkey.blob().size())
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validatePeerAddress(
        ReasonsGroupType &reason,
        const interface::types::AddressType &address) const {
      if (not(iroha::validator::isValidIpV4(address)
              or iroha::validator::isValidHostname(address))) {
        auto message =
            (boost::format("Wrongly formed peer address, passed value: '%s'")
             % address)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validateRoleId(
        ReasonsGroupType &reason,
        const interface::types::RoleIdType &role_id) const {
      if (not std::regex_match(role_id, name_pattern_)) {
        auto message =
            (boost::format("Wrongly formed role_id, passed value: '%s'")
             % role_id)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validateAccountName(
        ReasonsGroupType &reason,
        const interface::types::AccountNameType &account_name) const {
      if (not std::regex_match(account_name, name_pattern_)) {
        auto message =
            (boost::format("Wrongly formed account_name, passed value: '%s'")
             % account_name)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validateDomainId(
        ReasonsGroupType &reason,
        const interface::types::DomainIdType &domain_id) const {
      if (not std::regex_match(domain_id, name_pattern_)) {
        auto message =
            (boost::format("Wrongly formed domain_id, passed value: '%s'")
             % domain_id)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validateAssetName(
        ReasonsGroupType &reason,
        const interface::types::AssetNameType &asset_name) const {
      if (not std::regex_match(asset_name, name_pattern_)) {
        auto message =
            (boost::format("Wrongly formed asset_name, passed value: '%s'")
             % asset_name)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validateAccountDetailKey(
        ReasonsGroupType &reason,
        const interface::SetAccountDetail::AccountDetailKeyType &key) const {
      if (not std::regex_match(key, detail_key_pattern_)) {
        auto message =
            (boost::format("Wrongly formed key, passed value: '%s'") % key)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validatePrecision(
        ReasonsGroupType &reason,
        const interface::types::PrecisionType &precision) const {
      // define precision constraints
    }

    void FieldValidator::validatePermission(
        ReasonsGroupType &reason,
        const interface::types::PermissionNameType &permission_name) const {
      // define permission constraints
    }

    void FieldValidator::validatePermissions(
        ReasonsGroupType &reason,
        const interface::CreateRole::PermissionsType &permissions) const {
      if (permissions.empty()) {
        reason.second.push_back(
            "Permission set should contain at least one permission");
      }
    }

    void FieldValidator::validateQuorum(
        ReasonsGroupType &reason,
        const interface::types::QuorumType &quorum) const {
      // define quorum constraints
    }

    void FieldValidator::validateCreatorAccountId(
        ReasonsGroupType &reason,
        const interface::types::AccountIdType &account_id) const {
      if (not std::regex_match(account_id, account_id_pattern_)) {
        auto message =
            (boost::format(
                 "Wrongly formed creator_account_id, passed value: '%s'")
             % account_id)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validateCreatedTime(
        ReasonsGroupType &reason,
        const interface::types::TimestampType &timestamp) const {
      iroha::ts64_t now = iroha::time::now();

      if (now + future_gap_ < timestamp) {
        auto message = (boost::format("bad timestamp: sent from future, "
                                      "timestamp: %llu, now: %llu")
                        % timestamp % now)
                           .str();
        reason.second.push_back(std::move(message));
      }

      if (now > max_delay + timestamp) {
        auto message =
            (boost::format("bad timestamp: too old, timestamp: %llu, now: %llu")
             % timestamp % now)
                .str();
        reason.second.push_back(std::move(message));
      }
    }

    void FieldValidator::validateCounter(
        ReasonsGroupType &reason,
        const interface::types::CounterType &counter) const {
      if (counter <= 0) {
        auto message =
            (boost::format("Counter should be > 0, passed value: %d") % counter)
                .str();
        reason.second.push_back(message);
      }
    }

  }  // namespace validation
}  // namespace shared_model
