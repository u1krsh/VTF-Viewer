#!/bin/bash

# VTF-Viewer Release Build Script for Linux
# This script builds a release version of VTF-Viewer for Linux

# Exit immediately if a command exits with a non-zero status
set -e
# Exit if any command in a pipeline fails
set -o pipefail

echo "==================================="
echo "VTF-Viewer Linux Release Build"
echo "==================================="
echo ""

# Get the absolute path to the script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$SCRIPT_DIR"

# Verify we're in the correct directory
if [ ! -f "$PROJECT_ROOT/CMakeLists.txt" ]; then
    echo "ERROR: CMakeLists.txt not found!"
    echo "This script must be run from the VTF-Viewer project root directory."
    exit 1
fi

# Configuration
VERSION="1.0.0"
BUILD_DIR="$PROJECT_ROOT/build/release/linux"
RELEASE_NAME="VTF-Viewer-${VERSION}-Linux"
RELEASE_DIR="$BUILD_DIR/$RELEASE_NAME"

# Check for required tools
echo "Checking for required tools..."
MISSING_TOOLS=()

if ! command -v cmake &> /dev/null; then
    MISSING_TOOLS+=("cmake")
fi

if ! command -v make &> /dev/null; then
    MISSING_TOOLS+=("make")
fi

if ! command -v tar &> /dev/null; then
    MISSING_TOOLS+=("tar")
fi

if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
    echo "ERROR: Missing required tools: ${MISSING_TOOLS[*]}"
    echo "Please install them and try again."
    exit 1
fi

echo "All required tools found."
echo ""

# Clean previous builds with confirmation
if [ -d "$BUILD_DIR" ]; then
    echo "Previous build directory found at: $BUILD_DIR"
    read -p "Do you want to clean it? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Cleaning previous builds..."
        # Safety check: verify the path contains our expected directory structure
        if [[ "$BUILD_DIR" == *"/build/release/linux"* ]]; then
            rm -rf "$BUILD_DIR"
            echo "Cleaned successfully."
        else
            echo "ERROR: Build directory path looks suspicious: $BUILD_DIR"
            echo "Aborting for safety."
            exit 1
        fi
    else
        echo "Skipping clean. Build may fail if previous build exists."
    fi
fi

# Create build directories
echo "Creating build directories..."
mkdir -p "$RELEASE_DIR"
mkdir -p "$BUILD_DIR/build-temp"

# Navigate to build directory
cd "$BUILD_DIR/build-temp"

# Configure with CMake
echo ""
echo "Configuring CMake..."
if ! cmake -DCMAKE_BUILD_TYPE=Release "$PROJECT_ROOT"; then
    echo "ERROR: CMake configuration failed!"
    exit 1
fi

# Build
echo ""
echo "Building..."
NPROC=$(nproc 2>/dev/null || echo 4)
if ! make -j"$NPROC"; then
    echo "ERROR: Build failed!"
    exit 1
fi

# Verify executable was created
if [ ! -f "bin/VTF-Viewer" ]; then
    echo "ERROR: Executable not found at bin/VTF-Viewer"
    exit 1
fi

# Copy executable
echo ""
echo "Copying executable..."
cp bin/VTF-Viewer "$RELEASE_DIR/"
chmod +x "$RELEASE_DIR/VTF-Viewer"

# Copy resources
echo "Copying resources..."
if [ -f "$PROJECT_ROOT/LICENSE" ]; then
    cp "$PROJECT_ROOT/LICENSE" "$RELEASE_DIR/"
else
    echo "WARNING: LICENSE file not found"
fi

if [ -f "$PROJECT_ROOT/README.md" ]; then
    cp "$PROJECT_ROOT/README.md" "$RELEASE_DIR/"
else
    echo "WARNING: README.md file not found"
fi

# Create launcher script
echo "Creating launcher script..."
cat > "$RELEASE_DIR/VTF-Viewer.sh" << 'EOF'
#!/bin/bash
# VTF-Viewer Launcher Script
DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$DIR"
./VTF-Viewer "$@"
EOF

chmod +x "$RELEASE_DIR/VTF-Viewer.sh"

# Create archive
cd "$BUILD_DIR"
ARCHIVE_NAME="VTF-Viewer-${VERSION}-Linux-x86_64.tar.gz"
echo ""
echo "Creating archive..."
if ! tar -czf "$ARCHIVE_NAME" "$RELEASE_NAME/"; then
    echo "ERROR: Failed to create archive!"
    exit 1
fi

# Verify archive was created
if [ ! -f "$ARCHIVE_NAME" ]; then
    echo "ERROR: Archive not found!"
    exit 1
fi

ARCHIVE_SIZE=$(du -h "$ARCHIVE_NAME" | cut -f1)

echo ""
echo "==================================="
echo "Linux build complete!"
echo "==================================="
echo "Archive: $BUILD_DIR/$ARCHIVE_NAME"
echo "Size: $ARCHIVE_SIZE"
echo ""
echo "To test the build:"
echo "  cd $RELEASE_DIR"
echo "  ./VTF-Viewer.sh"
echo "==================================="
