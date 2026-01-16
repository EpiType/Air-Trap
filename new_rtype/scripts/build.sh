#!/bin/sh
set -e

BUILD_DIR=${BUILD_DIR:-build}
BUILD_TYPE=${1:-Release}

cmake -S . -B ${BUILD_DIR} -DCMAKE_BUILD_TYPE=${BUILD_TYPE}
cmake --build ${BUILD_DIR} -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
