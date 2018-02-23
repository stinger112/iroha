#!/usr/bin/env bash
# Building library files and generating wrapper with SWIG for node-gyp
# Saving them into buildlib, so node-gyp won't clean them
CURDIR="$(cd "$(dirname "$0")"; pwd)"
IROHA_HOME="$(dirname $(dirname $(dirname "${CURDIR}")))"

cmake -H$IROHA_HOME -Bbuild_cmake -DSWIG_NODE=ON -DSHARED_MODEL_DISABLE_COMPATIBILITY=ON -DSWIG_EXECUTABLE=/opt/swig/bin/swig
cmake --build build_cmake/ --target irohanode -- -j"$(getconf _NPROCESSORS_ONLN)";

# NPM install implicitly runs node-pre-gyp rebuild
npm install

# Generating protobuff files
mkdir -p pb/grpc
./node_modules/.bin/grpc_tools_node_protoc --proto_path=$IROHA_HOME/schema --js_out=import_style=commonjs,binary:./pb block.proto primitive.proto commands.proto queries.proto responses.proto endpoint.proto
./node_modules/.bin/grpc_tools_node_protoc --proto_path=$IROHA_HOME/schema --js_out=import_style=commonjs,binary:./pb/grpc --plugin=protoc-gen-grpc=./node_modules/grpc-tools/bin/grpc_node_plugin --grpc_out=./pb endpoint.proto yac.proto ordering.proto loader.proto

# TODO: link it properly in bindings.gyp for MacOS
if [ "$(uname)" == "Darwin" ]; then
 export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$(pwd)/build_cmake/shared_model/bindings
fi
