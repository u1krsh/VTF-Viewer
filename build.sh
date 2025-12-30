#!/bin/bash

# ValveExplorer Build Script

echo "==================================="
echo "ValveExplorer Build Script"
echo "==================================="
echo ""

# Check if Qt6 is installed
if ! pkg-config --exists Qt6Core; then
    echo "Qt6 is not installed. Installing..."
    echo "Please run: sudo apt-get install qt6-base-dev qt6-base-dev-tools"
    echo ""
    read -p "Press Enter after installing Qt6 to continue..."
fi

# Create build directory
echo "Creating build directory..."
mkdir -p build
cd build

# Run CMake
echo "Running CMake..."
cmake ..

if [ $? -ne 0 ]; then
    echo "CMake configuration failed!"
    exit 1
fi

# Build the project
echo "Building project..."
make -j$(nproc)

if [ $? -ne 0 ]; then
    echo "Build failed!"
    exit 1
fi

echo ""
echo "==================================="
echo "Build completed successfully!"
echo "==================================="
echo ""
echo "To run the application:"
echo "  ./bin/ValveExplorer"
echo ""
