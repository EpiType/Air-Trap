#!/bin/bash
# Build script for Linux/macOS

set -e

# Parse arguments
case "${1:-}" in
    clean)
        echo "Cleaning build artifacts..."
        rm -rf build/
        echo "✅ Clean complete!"
        exit 0
        ;;
    fclean)
        echo "Full clean (removing build and binaries)..."
        if [ -d !build ]; then
            ./build.sh clean
        fi
        rm -f r-type_client r-type_server
        echo "✅ Full clean complete!"
        exit 0
        ;;
esac

echo "Building Air-Trap..."

# Detect GCC version (prefer newer versions)
if command -v g++-15 &> /dev/null; then
    echo "✓ Using GCC 15 for C++23 support"
    GCC_VERSION=15
    CMAKE_C_COMPILER=gcc-15
    CMAKE_CXX_COMPILER=g++-15
elif command -v g++-14 &> /dev/null; then
    echo "✓ Using GCC 14 for C++23 support"
    GCC_VERSION=14
    CMAKE_C_COMPILER=gcc-14
    CMAKE_CXX_COMPILER=g++-14
elif command -v g++-13 &> /dev/null; then
    echo "⚠ Using GCC 13 (C++23 features may be limited)"
    GCC_VERSION=13
    CMAKE_C_COMPILER=gcc-13
    CMAKE_CXX_COMPILER=g++-13
else
    echo "⚠ Using system default GCC"
    GCC_VERSION=$(gcc -dumpversion | cut -d. -f1)
    CMAKE_C_COMPILER=gcc
    CMAKE_CXX_COMPILER=g++
fi

mkdir -p build
cd build

# Install dependencies with detected GCC version
conan install .. --output-folder=. --build=missing -s build_type=Release -s compiler.cppstd=23 -s compiler.version=$GCC_VERSION

# Configure with detected compiler
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=$CMAKE_C_COMPILER -DCMAKE_CXX_COMPILER=$CMAKE_CXX_COMPILER

# Build
cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Copy executables from bin/ to root
cp bin/r-type_client ../r-type_client
cp bin/r-type_server ../r-type_server

cd ..
echo ""
echo "✅ Build complete!"
echo "Server: ./r-type_server"
echo "Client: ./r-type_client"
