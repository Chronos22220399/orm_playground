#!/bin/bash

set -e

# 检查valgrind是否安装
if ! command -v valgrind &> /dev/null; then
    echo "Error: valgrind is not installed. Please install it first."
    echo "On macOS: brew install valgrind"
    echo "On Ubuntu/Debian: sudo apt-get install valgrind"
    exit 1
fi

# 构建目录
BUILD_DIR="build"
EXECUTABLE="orm"

# 检查是否已构建
if [ ! -f "$BUILD_DIR/$EXECUTABLE" ]; then
    echo "Building project with memory sanitizers..."
    mkdir -p $BUILD_DIR
    cd $BUILD_DIR
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    make -j$(nproc)
    cd ..
fi

echo "Running valgrind memory check..."
echo "=========================================="

valgrind \
    --leak-check=full \
    --show-leak-kinds=all \
    --track-origins=yes \
    --verbose \
    --log-file=valgrind_output.txt \
    ./$BUILD_DIR/$EXECUTABLE

echo "=========================================="
echo "Valgrind check completed. Output saved to valgrind_output.txt"

# 检查是否有内存泄漏
if grep -q "ERROR SUMMARY: 0 errors" valgrind_output.txt; then
    echo "✓ No memory leaks detected by valgrind"
else
    echo "⚠ Memory issues detected. Check valgrind_output.txt for details"
fi