#!/usr/bin/env bash

CURDIR="$(cd "$(dirname "$0")"; pwd)"
IROHA_HOME="$(dirname $(dirname $(dirname $(dirname "${CURDIR}"))))"

SHARED_INTERMEDIATE_DIR=$1
CMAKE_BUILD_DIR=$SHARED_INTERMEDIATE_DIR

echo ---------------------------------------------------------------------
echo "THIS SCRIPT BUILD SHARED MODEL WITH SWIG FOR NODEJS."
echo "OUTPUT DIRECTORY: $CMAKE_BUILD_DIR"
echo ---------------------------------------------------------------------

cmake -H$IROHA_HOME -B$CMAKE_BUILD_DIR -DSWIG_NODE=ON -DSHARED_MODEL_DISABLE_COMPATIBILITY=ON -DSWIG_EXECUTABLE=/opt/swig/bin/swig
cmake --build $CMAKE_BUILD_DIR --target irohanode -- -j"$(getconf _NPROCESSORS_ONLN)";

# TODO: link it properly in bindings.gyp for MacOS
# if [ "$(uname)" == "Darwin" ]; then
#  export DYLD_LIBRARY_PATH=$DYLD_LIBRARY_PATH:$(pwd)/lib/$MODULE_PATH
# fi

