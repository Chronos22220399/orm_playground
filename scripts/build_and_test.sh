#!/bin/bash

set -e

echo "Building project with memory sanitizers..."
echo "=========================================="

BUILD_DIR="build"
mkdir -p $BUILD_DIR
cd $BUILD_DIR

# Clean previous build
rm -rf *

# Configure with debug and sanitizers
cmake .. -DCMAKE_BUILD_TYPE=Debug
make -j$(nproc)

echo ""
echo "Build completed successfully!"
echo "=========================================="
echo ""
echo "Running memory leak detection tests..."
echo "=========================================="

# Run the executable (ASan will report issues)
./orm

echo ""
echo "=========================================="
echo "ASan tests completed."
echo ""
echo "For detailed Valgrind analysis, run:"
echo "  ./scripts/run_valgrind.sh"
echo ""
echo "Check MEMORY_ISSUES_CHECKLIST.md for detected issues."