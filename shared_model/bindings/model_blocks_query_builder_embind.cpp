#include <emscripten/bind.h>
#include <emscripten/val.h>

#include "model_proto.hpp"
#include "model_blocks_query_builder.hpp"

using namespace emscripten;
using namespace shared_model;
using namespace shared_model::bindings;
using namespace shared_model::proto;

using namespace shared_model::crypto;

EMSCRIPTEN_BINDINGS(model_blocks_query_builder) {
  typedef UnsignedWrapper<BlocksQuery> UnsignedWrapper;
  class_<UnsignedWrapper>("UnsignedBlocksQuery")
      .function("signAndAddSignature", &UnsignedWrapper::signAndAddSignature)
      .function("finish", &UnsignedWrapper::finish)
      .function("hash", &UnsignedWrapper::hash);

  typedef ModelProto<UnsignedWrapper> ModelProto;
  class_<ModelProto>("ModelProtoBlocksQuery")
      .constructor<UnsignedWrapper &>()
      .function("signAndAddSignature", &ModelProto::signAndAddSignature)
      .function("finish", &ModelProto::finish);

  /**
   * Top level ModelBlocksQueryBuilder class
   */
  class_<ModelBlocksQueryBuilder>("ModelBlocksQueryBuilder")
      .constructor<>()
      .function("createdTime",
                select_overload<ModelBlocksQueryBuilder(const emscripten::val &)>(
                    &ModelBlocksQueryBuilder::createdTime))
      .function("creatorAccountId", &ModelBlocksQueryBuilder::creatorAccountId)
      .function("queryCounter",
                select_overload<ModelBlocksQueryBuilder(const emscripten::val &)>(
                    &ModelBlocksQueryBuilder::queryCounter))
      .function("build", &ModelBlocksQueryBuilder::build);
}
