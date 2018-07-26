#!/usr/bin/env bash

CURDIR="$(cd "$(dirname "$0")"; pwd)"
SHARED_MODEL_HOME="$(dirname $(dirname $(dirname "${CURDIR}")))"
BINARY_PATH=${CURDIR}/../node_modules/.bin

echo "Generating Protobuf JS files..."

${BINARY_PATH}/grpc_tools_node_protoc \
--plugin="protoc-gen-grpc=${BINARY_PATH}/grpc_tools_node_protoc_plugin" \
--plugin="protoc-gen-ts=${BINARY_PATH}/protoc-gen-ts" \
--js_out="import_style=commonjs,binary:${CURDIR}/../pb" \
--grpc_out="${CURDIR}/../pb" \
--ts_out="service=true:${CURDIR}/../pb" \
--proto_path="${SHARED_MODEL_HOME}/schema" \
${SHARED_MODEL_HOME}/schema/*.proto
