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

#include "ametsuchi/impl/postgres_wsv_command.hpp"
#include "ametsuchi/impl/postgres_wsv_query.hpp"
#include "module/irohad/ametsuchi/ametsuchi_fixture.hpp"

namespace iroha {
  namespace ametsuchi {

    class WsvQueryCommandTest : public AmetsuchiTest {
     public:
      WsvQueryCommandTest() {
        domain.domain_id = "domain";
        domain.default_role = role;
        account.domain_id = domain.domain_id;
        account.account_id = "id@" + account.domain_id;
        account.quorum = 1;
        account.json_data = R"({"id@domain": {"key": "value"}})";
      }

      void SetUp() override {
        AmetsuchiTest::SetUp();
        postgres_connection = std::make_unique<pqxx::lazyconnection>(pgopt_);
        try {
          postgres_connection->activate();
        } catch (const pqxx::broken_connection &e) {
          FAIL() << "Connection to PostgreSQL broken: " << e.what();
        }
        wsv_transaction =
            std::make_unique<pqxx::nontransaction>(*postgres_connection);

        command = std::make_unique<PostgresWsvCommand>(*wsv_transaction);
        query = std::make_unique<PostgresWsvQuery>(*wsv_transaction);

        wsv_transaction->exec(init_);
      }

      std::string role = "role", permission = "permission";
      model::Account account;
      model::Domain domain;

      std::unique_ptr<pqxx::lazyconnection> postgres_connection;
      std::unique_ptr<pqxx::nontransaction> wsv_transaction;

      std::unique_ptr<WsvCommand> command;
      std::unique_ptr<WsvQuery> query;

      const std::string init_ = R"(
CREATE TABLE IF NOT EXISTS role (
    role_id character varying(45),
    PRIMARY KEY (role_id)
);
CREATE TABLE IF NOT EXISTS domain (
    domain_id character varying(164),
    default_role character varying(45) NOT NULL REFERENCES role(role_id),
    PRIMARY KEY (domain_id)
);
CREATE TABLE IF NOT EXISTS signatory (
    public_key bytea NOT NULL,
    PRIMARY KEY (public_key)
);
CREATE TABLE IF NOT EXISTS account (
    account_id character varying(197),
    domain_id character varying(164) NOT NULL REFERENCES domain,
    quorum int NOT NULL,
    transaction_count int NOT NULL DEFAULT 0,
    data JSONB,
    PRIMARY KEY (account_id)
);
CREATE TABLE IF NOT EXISTS account_has_signatory (
    account_id character varying(197) NOT NULL REFERENCES account,
    public_key bytea NOT NULL REFERENCES signatory,
    PRIMARY KEY (account_id, public_key)
);
CREATE TABLE IF NOT EXISTS peer (
    public_key bytea NOT NULL,
    address character varying(21) NOT NULL UNIQUE,
    PRIMARY KEY (public_key)
);
CREATE TABLE IF NOT EXISTS asset (
    asset_id character varying(197),
    domain_id character varying(164) NOT NULL REFERENCES domain,
    precision int NOT NULL,
    data json,
    PRIMARY KEY (asset_id)
);
CREATE TABLE IF NOT EXISTS account_has_asset (
    account_id character varying(197) NOT NULL REFERENCES account,
    asset_id character varying(197) NOT NULL REFERENCES asset,
    amount bigint NOT NULL,
    PRIMARY KEY (account_id, asset_id)
);
CREATE TABLE IF NOT EXISTS role_has_permissions (
    role_id character varying(45) NOT NULL REFERENCES role,
    permission_id character varying(45),
    PRIMARY KEY (role_id, permission_id)
);
CREATE TABLE IF NOT EXISTS account_has_roles (
    account_id character varying(197) NOT NULL REFERENCES account,
    role_id character varying(45) NOT NULL REFERENCES role,
    PRIMARY KEY (account_id, role_id)
);
CREATE TABLE IF NOT EXISTS account_has_grantable_permissions (
    permittee_account_id character varying(197) NOT NULL REFERENCES account,
    account_id character varying(197) NOT NULL REFERENCES account,
    permission_id character varying(45),
    PRIMARY KEY (permittee_account_id, account_id, permission_id)
);
)";
    };

    class RoleTest : public WsvQueryCommandTest {};

    TEST_F(RoleTest, InsertRoleWhenValidName) {
      ASSERT_TRUE(command->insertRole(role));
      auto roles = query->getRoles();
      ASSERT_TRUE(roles);
      ASSERT_EQ(1, roles->size());
      ASSERT_EQ(role, roles->front());
    }

    TEST_F(RoleTest, InsertRoleWhenInvalidName) {
      ASSERT_FALSE(command->insertRole(std::string(46, 'a')));

      auto roles = query->getRoles();
      ASSERT_TRUE(roles);
      ASSERT_EQ(0, roles->size());
    }

    class RolePermissionsTest : public WsvQueryCommandTest {
      void SetUp() override {
        WsvQueryCommandTest::SetUp();
        ASSERT_TRUE(command->insertRole(role));
      }
    };

    TEST_F(RolePermissionsTest, InsertRolePermissionsWhenRoleExists) {
      ASSERT_TRUE(command->insertRolePermissions(role, {permission}));

      auto permissions = query->getRolePermissions(role);
      ASSERT_TRUE(permissions);
      ASSERT_EQ(1, permissions->size());
      ASSERT_EQ(permission, permissions->front());
    }

    TEST_F(RolePermissionsTest, InsertRolePermissionsWhenNoRole) {
      auto new_role = role + " ";
      ASSERT_FALSE(command->insertRolePermissions(new_role, {permission}));

      auto permissions = query->getRolePermissions(new_role);
      ASSERT_TRUE(permissions);
      ASSERT_EQ(0, permissions->size());
    }

    class AccountTest : public WsvQueryCommandTest {
      void SetUp() override {
        WsvQueryCommandTest::SetUp();
        ASSERT_TRUE(command->insertRole(role));
        ASSERT_TRUE(command->insertDomain(domain));
      }
    };

    /**
     * @given inserted role, domain
     * @when insert account with filled json data
     * @then get account and check json data is the same
     */
    TEST_F(AccountTest, InsertAccountWithJSONData) {
      ASSERT_TRUE(command->insertAccount(account));
      auto acc = query->getAccount(account.account_id);
      ASSERT_TRUE(acc.has_value());
      ASSERT_EQ(account.json_data, acc.value().json_data);
    }

    /**
     * @given inserted role, domain, account
     * @when insert to account new json data
     * @then get account and check json data is the same
     */
    TEST_F(AccountTest, InsertNewJSONDataAccount) {
      ASSERT_TRUE(command->insertAccount(account));
      ASSERT_TRUE(command->setAccountKV(
          account.account_id, account.account_id, "id", "val"));
      auto acc = query->getAccount(account.account_id);
      ASSERT_TRUE(acc.has_value());
      ASSERT_EQ(R"({"id@domain": {"id": "val", "key": "value"}})",
                acc.value().json_data);
    }

    /**
     * @given inserted role, domain, account
     * @when insert to account new json data
     * @then get account and check json data is the same
     */
    TEST_F(AccountTest, InsertNewJSONDataToOtherAccount) {
      ASSERT_TRUE(command->insertAccount(account));
      ASSERT_TRUE(
          command->setAccountKV(account.account_id, "admin", "id", "val"));
      auto acc = query->getAccount(account.account_id);
      ASSERT_TRUE(acc.has_value());
      ASSERT_EQ(R"({"admin": {"id": "val"}, "id@domain": {"key": "value"}})",
                acc.value().json_data);
    }

    /**
     * @given inserted role, domain, account
     * @when insert to account new complex json data
     * @then get account and check json data is the same
     */
    TEST_F(AccountTest, InsertNewComplexJSONDataAccount) {
      ASSERT_TRUE(command->insertAccount(account));
      ASSERT_TRUE(command->setAccountKV(
          account.account_id, account.account_id, "id", "[val1, val2]"));
      auto acc = query->getAccount(account.account_id);
      ASSERT_TRUE(acc.has_value());
      ASSERT_EQ(R"({"id@domain": {"id": "[val1, val2]", "key": "value"}})",
                acc.value().json_data);
    }

    /**
     * @given inserted role, domain, account
     * @when update  json data in account
     * @then get account and check json data is the same
     */
    TEST_F(AccountTest, UpdateAccountJSONData) {
      ASSERT_TRUE(command->insertAccount(account));
      ASSERT_TRUE(command->setAccountKV(
          account.account_id, account.account_id, "key", "val2"));
      auto acc = query->getAccount(account.account_id);
      ASSERT_TRUE(acc.has_value());
      ASSERT_EQ(R"({"id@domain": {"key": "val2"}})", acc.value().json_data);
    }

    /**
     * @given database without needed account
     * @when performing query to retrieve non-existent account
     * @then getAccount will return nullopt
     */
    TEST_F(AccountTest, GetAccountInvalidWhenNotFound) {
      EXPECT_FALSE(query->getAccount("invalid account id"));
    }

    /**
     * @given database without needed account
     * @when performing query to retrieve non-existent account's details
     * @then getAccountDetail will return nullopt
     */
    TEST_F(AccountTest, GetAccountDetailInvalidWhenNotFound) {
      EXPECT_FALSE(query->getAccountDetail(
          "invalid account id", "invalid_creator", "invalid_detail"));
    }

    class AccountRoleTest : public WsvQueryCommandTest {
      void SetUp() override {
        WsvQueryCommandTest::SetUp();
        ASSERT_TRUE(command->insertRole(role));
        ASSERT_TRUE(command->insertDomain(domain));
        ASSERT_TRUE(command->insertAccount(account));
      }
    };

    TEST_F(AccountRoleTest, InsertAccountRoleWhenAccountRoleExist) {
      ASSERT_TRUE(command->insertAccountRole(account.account_id, role));

      auto roles = query->getAccountRoles(account.account_id);
      ASSERT_TRUE(roles);
      ASSERT_EQ(1, roles->size());
      ASSERT_EQ(role, roles->front());
    }

    TEST_F(AccountRoleTest, InsertAccountRoleWhenNoAccount) {
      auto account_id = account.account_id + " ";
      ASSERT_FALSE(command->insertAccountRole(account_id, role));

      auto roles = query->getAccountRoles(account_id);
      ASSERT_TRUE(roles);
      ASSERT_EQ(0, roles->size());
    }

    TEST_F(AccountRoleTest, InsertAccountRoleWhenNoRole) {
      auto new_role = role + " ";
      ASSERT_FALSE(command->insertAccountRole(account.account_id, new_role));

      auto roles = query->getAccountRoles(account.account_id);
      ASSERT_TRUE(roles);
      ASSERT_EQ(0, roles->size());
    }

    /**
     * @given inserted role, domain
     * @when insert and delete account role
     * @then role is detached
     */
    TEST_F(AccountRoleTest, DeleteAccountRoleWhenExist) {
      ASSERT_TRUE(command->insertAccountRole(account.account_id, role));
      ASSERT_TRUE(command->deleteAccountRole(account.account_id, role));
      auto roles = query->getAccountRoles(account.account_id);
      ASSERT_TRUE(roles);
      ASSERT_EQ(0, roles->size());
    }

    /**
     * @given inserted role, domain
     * @when no account exist
     * @then nothing is deleted
     */
    TEST_F(AccountRoleTest, DeleteAccountRoleWhenNoAccount) {
      ASSERT_TRUE(command->insertAccountRole(account.account_id, role));
      ASSERT_TRUE(command->deleteAccountRole("no", role));
      auto roles = query->getAccountRoles(account.account_id);
      ASSERT_TRUE(roles);
      ASSERT_EQ(1, roles->size());
    }

    /**
     * @given inserted role, domain
     * @when no role exist
     * @then nothing is deleted
     */
    TEST_F(AccountRoleTest, DeleteAccountRoleWhenNoRole) {
      ASSERT_TRUE(command->insertAccountRole(account.account_id, role));
      ASSERT_TRUE(command->deleteAccountRole(account.account_id, "no"));
      auto roles = query->getAccountRoles(account.account_id);
      ASSERT_TRUE(roles);
      ASSERT_EQ(1, roles->size());
    }

    class AccountGrantablePermissionTest : public WsvQueryCommandTest {
     public:
      AccountGrantablePermissionTest() {
        permittee_account = account;
        permittee_account.account_id = "id2@" + permittee_account.domain_id;
        permittee_account.quorum = 1;
      }

      void SetUp() override {
        WsvQueryCommandTest::SetUp();
        ASSERT_TRUE(command->insertRole(role));
        ASSERT_TRUE(command->insertDomain(domain));
        ASSERT_TRUE(command->insertAccount(account));
        ASSERT_TRUE(command->insertAccount(permittee_account));
      }

      model::Account permittee_account;
    };

    TEST_F(AccountGrantablePermissionTest,
           InsertAccountGrantablePermissionWhenAccountsExist) {
      ASSERT_TRUE(command->insertAccountGrantablePermission(
          permittee_account.account_id, account.account_id, permission));

      ASSERT_TRUE(query->hasAccountGrantablePermission(
          permittee_account.account_id, account.account_id, permission));
    }

    TEST_F(AccountGrantablePermissionTest,
           InsertAccountGrantablePermissionWhenNoPermitteeAccount) {
      auto permittee_account_id = permittee_account.account_id + " ";
      ASSERT_FALSE(command->insertAccountGrantablePermission(
          permittee_account_id, account.account_id, permission));

      ASSERT_FALSE(query->hasAccountGrantablePermission(
          permittee_account_id, account.account_id, permission));
    }

    TEST_F(AccountGrantablePermissionTest,
           InsertAccountGrantablePermissionWhenNoAccount) {
      auto account_id = account.account_id + " ";
      ASSERT_FALSE(command->insertAccountGrantablePermission(
          permittee_account.account_id, account_id, permission));

      ASSERT_FALSE(query->hasAccountGrantablePermission(
          permittee_account.account_id, account_id, permission));
    }

    TEST_F(AccountGrantablePermissionTest,
           DeleteAccountGrantablePermissionWhenAccountsPermissionExist) {
      ASSERT_TRUE(command->deleteAccountGrantablePermission(
          permittee_account.account_id, account.account_id, permission));

      ASSERT_FALSE(query->hasAccountGrantablePermission(
          permittee_account.account_id, account.account_id, permission));
    }

    class DeletePeerTest : public WsvQueryCommandTest {
     public:
      DeletePeerTest() {
        peer = model::Peer();
      }

      void SetUp() override {
        WsvQueryCommandTest::SetUp();
      }
      model::Peer peer;
    };

    /**
     * @given storage with peer
     * @when trying to delete existing peer
     * @then peer is successfully deleted
     */
    TEST_F(DeletePeerTest, DeletePeerValidWhenPeerExists) {
      ASSERT_TRUE(command->insertPeer(peer));

      EXPECT_TRUE(command->deletePeer(peer));
    }

    class GetAssetTest : public WsvQueryCommandTest {};

    /**
     * @given database without needed asset
     * @when performing query to retrieve non-existent asset
     * @then getAsset will return nullopt
     */
    TEST_F(GetAssetTest, GetAssetInvalidWhenAssetDoesNotExist) {
      EXPECT_FALSE(query->getAsset("invalid asset"));
    }

    class GetDomainTest : public WsvQueryCommandTest {};

    /**
     * @given database without needed domain
     * @when performing query to retrieve non-existent asset
     * @then getAsset will return nullopt
     */
    TEST_F(GetDomainTest, GetDomainInvalidWhenDomainDoesNotExist) {
      EXPECT_FALSE(query->getDomain("invalid domain"));
    }

    // Since mocking database is not currently possible, use SetUp to create
    // invalid database
    class DatabaseInvalidTest : public WsvQueryCommandTest {
      // skip database setup
      void SetUp() override {
        AmetsuchiTest::SetUp();
        postgres_connection = std::make_unique<pqxx::lazyconnection>(pgopt_);
        try {
          postgres_connection->activate();
        } catch (const pqxx::broken_connection &e) {
          FAIL() << "Connection to PostgreSQL broken: " << e.what();
        }
        wsv_transaction =
            std::make_unique<pqxx::nontransaction>(*postgres_connection);

        command = std::make_unique<PostgresWsvCommand>(*wsv_transaction);
        query = std::make_unique<PostgresWsvQuery>(*wsv_transaction);
      }
    };

    /**
     * @given not set up database
     * @when performing query to retrieve information from nonexisting tables
     * @then query will return nullopt
     */
    TEST_F(DatabaseInvalidTest, QueryInvalidWhenDatabaseInvalid) {
      EXPECT_FALSE(query->getAccount("some account"));
    }
  }  // namespace ametsuchi
}  // namespace iroha
