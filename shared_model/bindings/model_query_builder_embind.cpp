#include "model_query_builder.hpp"
#include <emscripten/bind.h>

using namespace emscripten;
using namespace shared_model;
using namespace shared_model::bindings;
using namespace shared_model::proto;

struct ModelQueryBuilderWrapper : public ModelQueryBuilder {
  void createdTime(const std::string& created_time) {
    std::cout << "Created time: " << created_time;
  }
};

EMSCRIPTEN_BINDINGS(model_query_builder)
{
  // class_<Query>("Query")
  // // .constructor<>()
  // .function("get", &Query::get)
  // .function("creatorAccountId", &Query::creatorAccountId)
  // .function("queryCounter", &Query::queryCounter)
  // .function("blob", &Query::blob)
  // .function("payload", &Query::payload)
  // // Signable override
  // .function("signatures", &Query::signatures)
  // .function("addSignature", &Query::addSignature)
  // .function("clearSignatures", &Query::clearSignatures)
  // .function("createdTime", &Query::createdTime);

  // TODO: Unsigned wrapper is a template and can have many base types
  // typedef UnsignedWrapper<Query> UnsignedWrapperQuery;

  // class_<UnsignedWrapperQuery>("UnsignedWrapper")
  // // .constructor<>()
  // .function("signAndAddSignature", &UnsignedWrapperQuery::signAndAddSignature)
  // .function("hash", &UnsignedWrapperQuery::hash);

  /**
   * Top level ModelQueryBuilder class
   **/
  class_<ModelQueryBuilder>("ModelQueryBuilder")
  .constructor<>()
  .function("createdTime", &ModelQueryBuilder::createdTime)
  .function("creatorAccountId", &ModelQueryBuilder::creatorAccountId)
  .function("queryCounter", &ModelQueryBuilder::queryCounter)
  .function("getAccount", &ModelQueryBuilder::getAccount)
  .function("getSignatories", &ModelQueryBuilder::getSignatories)
  .function("getAccountTransactions", &ModelQueryBuilder::getAccountTransactions)
  .function("getAccountAssetTransactions", &ModelQueryBuilder::getAccountAssetTransactions)
  .function("getAccountAssets", &ModelQueryBuilder::getAccountAssets)
  .function("getRoles", &ModelQueryBuilder::getRoles)
  .function("getAssetInfo", &ModelQueryBuilder::getAssetInfo)
  .function("getRolePermissions", &ModelQueryBuilder::getRolePermissions)
  .function("getTransactions", &ModelQueryBuilder::getTransactions)
  .function("getAccountDetail", &ModelQueryBuilder::getAccountDetail)
  .function("build", &ModelQueryBuilder::build);

  class_<ModelQueryBuilderWrapper, base<ModelQueryBuilder>>("ModelQueryBuilder")
  .constructor<>()
  .function("createdTime", &ModelQueryBuilderWrapper::createdTime)
  .function("creatorAccountId", &ModelQueryBuilder::creatorAccountId)
  .function("queryCounter", &ModelQueryBuilder::queryCounter)
  .function("getAccount", &ModelQueryBuilder::getAccount)
  .function("getSignatories", &ModelQueryBuilder::getSignatories)
  .function("getAccountTransactions", &ModelQueryBuilder::getAccountTransactions)
  .function("getAccountAssetTransactions", &ModelQueryBuilder::getAccountAssetTransactions)
  .function("getAccountAssets", &ModelQueryBuilder::getAccountAssets)
  .function("getRoles", &ModelQueryBuilder::getRoles)
  .function("getAssetInfo", &ModelQueryBuilder::getAssetInfo)
  .function("getRolePermissions", &ModelQueryBuilder::getRolePermissions)
  .function("getTransactions", &ModelQueryBuilder::getTransactions)
  .function("getAccountDetail", &ModelQueryBuilder::getAccountDetail)
  .function("build", &ModelQueryBuilder::build);
}