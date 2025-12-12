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

mkdir -p build
cd build

conan install .. --output-folder=. --build=missing -s build_type=Release -s compiler.cppstd=23

cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Release 

cmake --build . --config Release -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

cp client/r-type_client ../r-type_client
cp server/r-type_server ../r-type_server

cd ..
echo ""
echo "✅ Build complete!"
echo "Server: ./build/r-type_server"
echo "Client: ./build/r-type_client"
