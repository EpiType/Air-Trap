#!/bin/bash
# Build script for Linux/macOS

set -e

echo "Building Air-Trap..."

# Create build directory
mkdir -p build
cd build

# Install dependencies with Conan
conan install .. --output-folder=. --build=missing -s build_type=Release

# Configure with CMake using the Conan toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc-14 -DCMAKE_CXX_COMPILER=g++-14

# Build
cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

cd ..
echo ""
echo "✅ Build complete!"
echo "Server: ./build/r-type_server"
echo "Client: ./build/r-type_client"
