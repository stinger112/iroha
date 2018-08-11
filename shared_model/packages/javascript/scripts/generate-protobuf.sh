#!/usr/bin/env bash

CURDIR="$(cd "$(dirname "$0")"; pwd)"
SHARED_MODEL_HOME="$(dirname $(dirname $(dirname "${CURDIR}")))"
BINARY_PATH=${CURDIR}/../node_modules/.bin

echo "Generating Protobuf JS files..."

# Generate common protobuf files
${BINARY_PATH}/grpc_tools_node_protoc \
--js_out="import_style=commonjs,binary:${CURDIR}/../pb" \
--proto_path="${SHARED_MODEL_HOME}/schema" \
${SHARED_MODEL_HOME}/schema/*.proto

# Generate GRPC Service Stubs
${BINARY_PATH}/grpc_tools_node_protoc \
--plugin="protoc-gen-grpc=${BINARY_PATH}/grpc_tools_node_protoc_plugin" \
--grpc_out="${CURDIR}/../pb" \
--proto_path="${SHARED_MODEL_HOME}/schema" \
${SHARED_MODEL_HOME}/schema/endpoint.proto

# Generate GRPC-WEB Service Stubs
${BINARY_PATH}/grpc_tools_node_protoc \
--plugin="protoc-gen-ts=${BINARY_PATH}/protoc-gen-ts" \
--ts_out="service=true:${CURDIR}/../pb" \
--proto_path="${SHARED_MODEL_HOME}/schema" \
${SHARED_MODEL_HOME}/schema/endpoint.proto

# Generate TypeScript declarations
${BINARY_PATH}/grpc_tools_node_protoc \
--plugin="protoc-gen-ts=${BINARY_PATH}/protoc-gen-ts" \
--ts_out="${CURDIR}/../pb" \
--proto_path="${SHARED_MODEL_HOME}/schema" \
${SHARED_MODEL_HOME}/schema/*.proto
