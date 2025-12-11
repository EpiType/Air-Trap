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

# Create build directory
mkdir -p build
cd build

# Install dependencies with Conan
conan install .. --output-folder=. --build=missing -s build_type=Release

# Configure with CMake using the Conan toolchain
cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc-14 -DCMAKE_CXX_COMPILER=g++-14

# Build
cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

cp client/r-type_client ../r-type_client
cp server/r-type_server ../r-type_server

cd ..
echo ""
echo "✅ Build complete!"
echo "Server: ./r-type_server"
echo "Client: ./r-type_client"
echo ""
echo "For cleaning, run: ./scripts/build.sh clean or ./scripts/build.sh fclean"