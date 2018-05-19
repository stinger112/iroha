#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "model_transaction_builder.hpp"

using namespace emscripten;
using namespace shared_model;
using namespace shared_model::bindings;
using namespace shared_model::proto;

using namespace shared_model::crypto;

EMSCRIPTEN_BINDINGS(model_transaction_builder)
{
  class_<Transaction>("Transaction")
  .function("creatorAccountId", &Transaction::creatorAccountId)
//   .function("commands", &Transaction::commands)
  .function("blob", &Transaction::blob)
  .function("payload", &Transaction::payload)
//   .function("signatures", &Transaction::getSignatures)
  .function("addSignature", &Transaction::addSignature)
  .function("clearSignatures", &Transaction::clearSignatures)
  .function("createdTime", &Transaction::getCreatedTime);

  // TODO: Unsigned wrapper is a template and can have many base types
  typedef UnsignedWrapper<Transaction> UnsignedWrapperTransaction;
  
  class_<UnsignedWrapperTransaction>("UnsignedWrapperTransaction")
  .function("signAndAddSignature", &UnsignedWrapperTransaction::signAndAddSignature)
  .function("hash", &UnsignedWrapperTransaction::hash);

  /**
   * Top level ModelTransactionBuilder class
   **/
  class_<ModelTransactionBuilder>("ModelTransactionBuilder")
  .constructor<>()
  .function("creatorAccountId", &ModelTransactionBuilder::creatorAccountId)
  .function("createdTime", select_overload<ModelTransactionBuilder(const emscripten::val&)>(&ModelTransactionBuilder::createdTime))
  .function("addAssetQuantity", &ModelTransactionBuilder::addAssetQuantity)
  .function("addPeer", &ModelTransactionBuilder::addPeer)
  .function("addSignatory", &ModelTransactionBuilder::addSignatory)
  .function("removeSignatory", &ModelTransactionBuilder::removeSignatory)
  .function("appendRole", &ModelTransactionBuilder::appendRole)
  .function("createAsset", &ModelTransactionBuilder::createAsset)
  .function("createAccount", &ModelTransactionBuilder::createAccount)
  .function("createDomain", &ModelTransactionBuilder::createDomain)
  .function("createRole", &ModelTransactionBuilder::createRole)
  .function("detachRole", &ModelTransactionBuilder::detachRole)
  .function("grantPermission", &ModelTransactionBuilder::grantPermission)
  .function("revokePermission", &ModelTransactionBuilder::revokePermission)
  .function("setAccountDetail", &ModelTransactionBuilder::setAccountDetail)
  .function("setAccountQuorum", &ModelTransactionBuilder::setAccountQuorum)
  .function("subtractAssetQuantity", &ModelTransactionBuilder::subtractAssetQuantity)
  .function("transferAsset", &ModelTransactionBuilder::transferAsset)
  .function("build", &ModelTransactionBuilder::build);
}