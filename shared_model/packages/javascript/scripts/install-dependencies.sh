#!/usr/bin/env bash

git clone https://gitlab.kitware.com/cmake/cmake.git /tmp/cmake; \
    (cd /tmp/cmake ; git checkout 64130a7e793483e24c1d68bdd234f81d5edb2d51); \
    (cd /tmp/cmake ; /tmp/cmake/bootstrap --parallel=${PARALLELISM} --enable-ccache); \
    make -j${PARALLELISM} -C /tmp/cmake; \
    make -C /tmp/cmake install; \
    rm -rf /tmp/cmake

git clone --depth 1 --branch rel-3.0.12 https://github.com/swig/swig
