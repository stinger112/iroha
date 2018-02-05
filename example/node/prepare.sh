#!/usr/bin/env bash
CURDIR="$(cd "$(dirname "$0")"; pwd)"
IROHA_HOME="$(dirname $(dirname "${CURDIR}"))"

cmake -H$IROHA_HOME -Bbuild -DSWIG_NODE=ON;
cmake --build build/ --target irohanode -- -j"$(getconf _NPROCESSORS_ONLN)"

node-gyp configure build

[ ! -d "./pb" ] && mkdir pb
protoc --proto_path=../../schema --js_out=import_style=commonjs,binary:./pb block.proto primitive.proto commands.proto queries.proto responses.proto endpoint.proto
protoc --proto_path=../../schema --js_out=import_style=commonjs,binary:./ --plugin=protoc-gen-grpc=./node_modules/grpc-tools/bin/grpc_node_plugin --grpc_out=./pb endpoint.proto yac.proto ordering.proto loader.proto
