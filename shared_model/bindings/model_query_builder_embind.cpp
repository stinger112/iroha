#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "model_query_builder.hpp"

using namespace emscripten;
using namespace shared_model;
using namespace shared_model::bindings;
using namespace shared_model::proto;

using namespace shared_model::crypto;

EMSCRIPTEN_BINDINGS(model_query_builder)
{
  register_vector<Hash>("HashVector");
  class_<Hash, base<Blob>>("Hash")
  .constructor<const std::string&>()
  .function("toString", &Hash::toString);

  class_<Query>("Query")
  // .function("get", &Query::get)
  .function("creatorAccountId", &Query::creatorAccountId)
  .function("queryCounter", &Query::getQueryCounter)
  .function("blob", &Query::blob)
  .function("payload", &Query::payload)
  // .function("signatures", &Query::signatures)
  .function("addSignature", &Query::addSignature)
  .function("createdTime", &Query::getCreatedTime);

  // TODO: Unsigned wrapper is a template and can have many base types
  typedef UnsignedWrapper<Query> UnsignedWrapperQuery;
  
  class_<UnsignedWrapperQuery>("UnsignedWrapper")
  .function("signAndAddSignature", &UnsignedWrapperQuery::signAndAddSignature)
  .function("hash", &UnsignedWrapperQuery::hash);

  /**
   * Top level ModelQueryBuilder class
   */
  class_<ModelQueryBuilder>("ModelQueryBuilder")
  .constructor<>()
  .function("createdTime", select_overload<ModelQueryBuilder(const emscripten::val&)>(&ModelQueryBuilder::createdTime))
  .function("creatorAccountId", &ModelQueryBuilder::creatorAccountId)
  .function("queryCounter", select_overload<ModelQueryBuilder(const emscripten::val&)>(&ModelQueryBuilder::queryCounter))
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
