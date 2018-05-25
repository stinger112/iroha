#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "model_query_builder.hpp"

using namespace emscripten;
using namespace shared_model;
using namespace shared_model::bindings;
using namespace shared_model::proto;

using namespace shared_model::crypto;

// struct ModelQueryBuilderWrapper : public ModelQueryBuilder {
//   void createdTime(const std::string& created_time) {
//     std::cout << "Created time: " << created_time;
//   }
// };

// struct ModelQueryBuilderWrapper : public wrapper<ModelQueryBuilder> {
//     EMSCRIPTEN_WRAPPER(ModelQueryBuilderWrapper);

//     ModelQueryBuilder invoke(const std::string& str) {
//       std::cout << str;

//       return call<ModelQueryBuilder>("createdTime", 0123);
//     }
// };

// TODO: Need to return std::map with signatures
// using namespace shared_model::interface;
// EMSCRIPTEN_BINDINGS(model_signatures)
// {
//   class_<Signature>("Signature")
// }

EMSCRIPTEN_BINDINGS(model_query_builder)
{
  // TODO: Move Hash to module_crypto because it's common class
  class_<Hash, base<Blob>>("Hash")
  .constructor<const std::string&>()
  .function("toString", &Hash::toString);

  register_vector<Hash>("HashVector");

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
  // .constructor<UnsignedWrapperQuery>()
  .function("signAndAddSignature", &UnsignedWrapperQuery::signAndAddSignature)
  .function("hash", &UnsignedWrapperQuery::hash);

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
