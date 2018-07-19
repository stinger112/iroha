#!/usr/bin/env bash

CURDIR="$(cd "$(dirname "$0")"; pwd)"
SHARED_MODEL_HOME="$(dirname $(dirname $(dirname "${CURDIR}")))"

# Check if we inside Iroha repository
if [ -d "${SHARED_MODEL_HOME}/schema" ]; then
	echo "Generating Protobuf JS files..."
	
	./node_modules/.bin/grpc_tools_node_protoc --proto_path=${SHARED_MODEL_HOME}/schema \
	--plugin=protoc-gen-grpc=./node_modules/grpc-tools/bin/grpc_node_plugin \
	--js_out=import_style=commonjs,binary:${CURDIR}/../pb \
	--grpc_out=${CURDIR}/../pb \
	block.proto commands.proto endpoint.proto primitive.proto proposal.proto qry_responses.proto queries.proto transaction.proto
else
  echo "This is standalone installation. No protobufs generated."
fi
