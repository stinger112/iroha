#!/bin/bash

CURDIR="$(cd "$(dirname "$0")"; pwd)"
SHARED_MODEL_HOME="$(dirname $(dirname $(dirname "${CURDIR}")))"

# Are we inside Iroha repository?
if [ -d "${SHARED_MODEL_HOME}/schema" ]; then
  echo "Prepare the package..."

  sh $CURDIR/clean.sh
  sh $CURDIR/generate-protobuf.sh

  echo "Copying compiled library files..."

  mkdir -p $CURDIR/../lib
  cp -u ${SHARED_MODEL_HOME}/build/bindings/irohalib.js $CURDIR/../lib

  echo "All preparations have been finished successfully!"
else
  echo "This is standalone installation. No preparations needed."
fi
