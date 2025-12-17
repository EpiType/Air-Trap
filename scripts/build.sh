#!/bin/bash
# Build script for Linux/macOS

set -e

# Detect GCC version FIRST (needed for all build types)
if command -v g++-15 &> /dev/null; then
    GCC_VERSION=15
    CMAKE_C_COMPILER=gcc-15
    CMAKE_CXX_COMPILER=g++-15
elif command -v g++-14 &> /dev/null; then
    GCC_VERSION=14
    CMAKE_C_COMPILER=gcc-14
    CMAKE_CXX_COMPILER=g++-14
elif command -v g++-13 &> /dev/null; then
    GCC_VERSION=13
    CMAKE_C_COMPILER=gcc-13
    CMAKE_CXX_COMPILER=g++-13
else
    GCC_VERSION=$(gcc -dumpversion | cut -d. -f1)
    CMAKE_C_COMPILER=gcc
    CMAKE_CXX_COMPILER=g++
fi

# Parse arguments
case "${1:-}" in
    debug)
        echo "Building Air-Trap in Debug mode..."
        echo "✓ Using GCC $GCC_VERSION"
        mkdir -p build
        cd build
        conan install .. --build=missing --output-folder=. -s build_type=Debug -s compiler.cppstd=23 -s compiler.version=$GCC_VERSION
        cmake .. -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER=$CMAKE_C_COMPILER -DCMAKE_CXX_COMPILER=$CMAKE_CXX_COMPILER
        cmake --build . --config Debug -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
        
        # Copy debug binaries to root with _debug suffix
        cp bin/r-type_client ../r-type_client_debug
        cp bin/r-type_server ../r-type_server_debug
        
        cd ..
        echo ""
        echo "✅ Debug build complete!"
        echo ""
        echo "🐛 Debug binaries created:"
        echo "   Server: ./r-type_server_debug"
        echo "   Client: ./r-type_client_debug"
        echo ""
        echo "⚠️  WARNING: Use the _debug binaries for debug keys!"
        echo "   ./r-type_client        ← Release (NO debug keys)"
        echo "   ./r-type_client_debug  ← Debug (debug keys J/I/K/L/M enabled)"
        echo ""
        echo "🎮 Debug Keys Available:"
        echo "   J - Spawn enemy at mouse position"
        echo "   I - Spawn second type of enemy at mouse position"
        echo "   K - Kill enemy at index 0"
        echo "   L - Spawn projectile at mouse position"
        echo "   M - Kill projectile at index 0"
        exit 0
        ;;
    test)
        echo "Running tests..."
        if [ ! -d build ]; then
            echo "❌ Build directory not found. Run './build.sh' first."
            exit 1
        fi
        cd build
        ctest --output-on-failure
        echo "✅ Tests complete!"
        exit 0
        ;;
    coverage)
        echo "Running tests with coverage..."
        if [ ! -d build ]; then
            echo "❌ Build directory not found. Building with coverage..."
            ./scripts/build.sh clean
        fi
        
        # Build with coverage
        mkdir -p build
        cd build
        conan install .. --build=missing --output-folder=. -s build_type=Debug
        cmake .. -DCMAKE_BUILD_TYPE=Debug -DCMAKE_TOOLCHAIN_FILE=conan_toolchain.cmake -DENABLE_COVERAGE=ON
        cmake --build . -j$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
        
        # Run tests
        ctest --output-on-failure
        
        # Generate coverage report
        if command -v gcovr &> /dev/null; then
            echo ""
            echo "📊 Coverage Report:"
            gcovr --root .. --filter '../common/src/.*' --print-summary
            gcovr --root .. --filter '../common/src/.*' --xml coverage.xml
            echo "✅ Coverage report generated: build/coverage.xml"
        else
            echo "⚠ gcovr not installed. Install with: pip install gcovr"
        fi
        cd ..
        exit 0
        ;;
    clean)
        echo "Cleaning build artifacts..."
        rm -rf build/
        echo "✅ Clean complete!"
        exit 0
        ;;
    fclean)
        echo "Full clean (removing build and binaries)..."
        if [ -d build ]; then
            ./scripts/build.sh clean
        fi
        rm -f r-type_client r-type_server r-type_client_debug r-type_server_debug
        echo "✅ Full clean complete!"
        exit 0
        ;;
    help|--help|-h)
        echo "Usage: ./scripts/build.sh [COMMAND]"
        echo ""
        echo "Commands:"
        echo "  (none)    Build the project in Release mode"
        echo "  test      Run unit tests"
        echo "  coverage  Build with coverage and run tests with coverage report"
        echo "  clean     Remove build directory"
        echo "  fclean    Remove build directory and binaries"
        echo "  help      Show this help message"
        exit 0
        ;;
esac

echo "Building Air-Trap in Release mode..."
echo "✓ Using GCC $GCC_VERSION"

mkdir -p build
cd build

# Install dependencies with detected GCC version
conan install .. --build=missing --output-folder=. -s build_type=Release -s compiler.cppstd=23 -s compiler.version=$GCC_VERSION

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
echo ""
echo "💡 Run debug build with: ./scripts/build.sh debug"
echo "💡 Run tests with: ./scripts/build.sh test"
echo "💡 Run coverage with: ./scripts/build.sh coverage"
