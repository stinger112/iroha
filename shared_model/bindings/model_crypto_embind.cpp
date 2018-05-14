#include "model_crypto.hpp"
#include "model_query_builder.hpp"
#include <emscripten/bind.h>

using namespace emscripten;
using namespace shared_model::bindings;
using namespace shared_model::crypto;

EMSCRIPTEN_BINDINGS(model_crypto)
{
  // value_object<Keypair>("Keypair")
  //     .field("publicKey", &Keypair::public_key_)
  //     .field("privateKey", &Keypair::private_key_);

  // TODO: Can we create wrapper for std::exception?
  // class_<std::exception>("IrohaException")
  // .constructor<std::exception>();
  // .function("what", &std::invalid_argument::what);

  // class_<std::invalid_argument>("InvalidArgumentException")
  // .constructor<std::string>()
  // .function("what", &std::invalid_argument::what, allow_raw_pointers());

  class_<Blob>("Blob")
  .constructor<std::string>()
  .function("fromHexString", &Blob::fromHexString)
  .function("blob", &Blob::blob)
  .function("hex", &Blob::hex)
  .function("toString", &Blob::toString);

  class_<PublicKey, base<Blob>>("PublicKey")
  .constructor<std::string>()
  .function("toString", &PublicKey::toString);

  class_<PrivateKey, base<Blob>>("PrivateKey")
  .constructor<std::string>()
  .function("toString", &PrivateKey::toString);

  class_<Keypair>("Keypair")
  .constructor<PublicKey, PrivateKey>()
  .function("publicKey", &Keypair::publicKey)
  .function("privateKey", &Keypair::privateKey);

  /**
   * Top level ModelCrypto class
   **/
  class_<ModelCrypto>("ModelCrypto")
  .constructor<>()
  .function("generateKeypair", &ModelCrypto::generateKeypair)
  .function("fromPrivateKey", &ModelCrypto::fromPrivateKey)
  .function("convertFromExisting", &ModelCrypto::convertFromExisting);
}


// namespace shared_model {
//   namespace bindings {
//     class ModelQueryBuilder {}
// }

using namespace shared_model::proto;

EMSCRIPTEN_BINDINGS(model_query_builder)
{
  // TODO: Need to check how it works
  value_object<shared_model::interface::types::TimestampType>("TimestampType");

  class_<Query>("Query")
  // .constructor<>()
  .function("get", &Query::get)
  .function("creatorAccountId", &Query::creatorAccountId)
  .function("queryCounter", &Query::queryCounter)
  .function("blob", &Query::blob)
  .function("payload", &Query::payload)
  // Signable override
  .function("signatures", &Query::signatures)
  .function("addSignature", &Query::addSignature)
  .function("clearSignatures", &Query::clearSignatures)
  .function("createdTime", &Query::createdTime);

  // TODO: Unsigned wrapper is a template and can have many base types
  typedef UnsignedWrapper<Query> UnsignedWrapperQuery;

  class_<UnsignedWrapperQuery>("UnsignedWrapper")
    // .constructor<>()
    .function("signAndAddSignature", &UnsignedWrapperQuery::signAndAddSignature)
    .function("hash", &UnsignedWrapperQuery::hash);

  /**
   * Top level ModelQueryBuilder class
   **/
  class_<ModelQueryBuilder>("ModelQueryBuilder")
  .constructor<>()
  // .function("createdTime", &ModelQueryBuilder::createdTime, optional_override(
  //   [](ModelQueryBuilder& self, const uint& created_time){
  //     return self.ModelQueryBuilder::createdTime(created_time);
  // }))
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
}
