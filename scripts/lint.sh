#!/bin/bash
# Script to run clang-tidy on all C++ source files

echo "Running clang-tidy..."

if [ ! -f "build/compile_commands.json" ]; then
    echo "Error: compile_commands.json not found. Please build the project first."
    exit 1
fi

find . -type f \( -name "*.cpp" -o -name "*.hpp" \) \
    -not -path "*/build/*" \
    -not -path "*/cmake-build-*/*" \
    -not -path "*/.conan/*" \
    -exec clang-tidy -p build {} \;

echo "clang-tidy complete!"
