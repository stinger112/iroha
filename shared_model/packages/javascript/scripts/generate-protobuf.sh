#!/usr/bin/env bash

CURDIR="$(cd "$(dirname "$0")"; pwd)"
IROHA_HOME="$(dirname $(dirname $(dirname $(dirname "${CURDIR}"))))"

echo -------------------------
echo "IROHA_HOME: $IROHA_HOME"
echo "Generating JS protobuff files..."

# Generating protobuff files
mkdir -p pb/grpc
./node_modules/.bin/grpc_tools_node_protoc --proto_path=$IROHA_HOME/schema --js_out=import_style=commonjs,binary:./pb block.proto primitive.proto commands.proto queries.proto responses.proto endpoint.proto
./node_modules/.bin/grpc_tools_node_protoc --proto_path=$IROHA_HOME/schema --js_out=import_style=commonjs,binary:./pb/grpc --plugin=protoc-gen-grpc=./node_modules/grpc-tools/bin/grpc_node_plugin --grpc_out=./pb endpoint.proto yac.proto ordering.proto loader.proto