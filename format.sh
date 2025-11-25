#!/bin/bash
# Script to format all C++ source files

echo "Formatting C++ files..."

find . -type f \( -name "*.cpp" -o -name "*.hpp" -o -name "*.h" -o -name "*.cc" \) \
    -not -path "*/build/*" \
    -not -path "*/cmake-build-*/*" \
    -not -path "*/.conan/*" \
    -exec clang-format -i {} \;

echo "Formatting complete!"
