#!/bin/bash

echo "=== PulseStation Build Script (Qt5 - Linux) ==="

if ! command -v cmake &> /dev/null; then
    echo "ERROR: CMake not found."
    echo "Install: sudo apt install cmake"
    exit 1
fi

if ! command -v qmake &> /dev/null; then
    echo "ERROR: Qt5 not found."
    echo "Install: sudo apt install qtbase5-dev"
    exit 1
fi

QT_PATH=$(qmake -query QT_INSTALL_PREFIX)
echo "Qt5 Path: $QT_PATH"
echo

mkdir -p build
cd build

echo "Configuring..."
cmake .. -DCMAKE_PREFIX_PATH=$QT_PATH

if [ $? -ne 0 ]; then
    echo "ERROR: CMake configuration failed"
    exit 1
fi

echo "Building..."
cmake --build . -- -j$(nproc)

if [ $? -ne 0 ]; then
    echo "ERROR: Build failed"
    exit 1
fi

echo
echo "=== Build Successful! ==="
echo "Run: ./build/PulseStation"
