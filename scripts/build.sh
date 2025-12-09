#!/bin/bash
# Build script for Linux/macOS

set -e

echo "Building Air-Trap..."

mkdir -p build
cd build

conan install .. --output-folder=. --build=missing -s build_type=Release -s compiler.cppstd=23

cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release 

cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

cd ..
echo ""
echo "✅ Build complete!"
echo "Server: ./build/r-type_server"
echo "Client: ./build/r-type_client"