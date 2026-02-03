#!/bin/bash

# VTF-Viewer Release Build Script for macOS
# This script builds a release version of VTF-Viewer for macOS

# Exit immediately if a command exits with a non-zero status
set -e
# Exit if any command in a pipeline fails
set -o pipefail

echo "==================================="
echo "VTF-Viewer macOS Release Build"
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
BUILD_DIR="$PROJECT_ROOT/build/release/macos"
RELEASE_NAME="VTF-Viewer-${VERSION}-macOS"
RELEASE_DIR="$BUILD_DIR/$RELEASE_NAME"
APP_NAME="VTF-Viewer.app"

# Check for required tools
echo "Checking for required tools..."
MISSING_TOOLS=()

if ! command -v cmake &> /dev/null; then
    MISSING_TOOLS+=("cmake")
fi

if ! command -v make &> /dev/null; then
    MISSING_TOOLS+=("make")
fi

if ! command -v hdiutil &> /dev/null; then
    MISSING_TOOLS+=("hdiutil")
fi

if ! command -v macdeployqt &> /dev/null; then
    MISSING_TOOLS+=("macdeployqt")
fi

if [ ${#MISSING_TOOLS[@]} -ne 0 ]; then
    echo "ERROR: Missing required tools: ${MISSING_TOOLS[*]}"
    echo "Please install them and try again."
    echo ""
    echo "Hint: Install Qt6 via Homebrew: brew install qt@6"
    exit 1
fi

echo "All required tools found."
echo ""

# Try to find Qt installation
QT_PATH=""
if command -v brew &> /dev/null; then
    QT_PATH=$(brew --prefix qt@6 2>/dev/null || echo "")
fi

if [ -z "$QT_PATH" ]; then
    echo "WARNING: Could not auto-detect Qt6 path."
    echo "CMake will use system default Qt path."
    echo ""
fi

# Clean previous builds with confirmation
if [ -d "$BUILD_DIR" ]; then
    echo "Previous build directory found at: $BUILD_DIR"
    read -p "Do you want to clean it? (y/N): " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        echo "Cleaning previous builds..."
        # Safety check: verify the path contains our expected directory structure
        if [[ "$BUILD_DIR" == *"/build/release/macos"* ]]; then
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
if [ -n "$QT_PATH" ]; then
    echo "Using Qt from: $QT_PATH"
    if ! cmake -DCMAKE_BUILD_TYPE=Release \
               -DCMAKE_PREFIX_PATH="$QT_PATH" \
               "$PROJECT_ROOT"; then
        echo "ERROR: CMake configuration failed!"
        exit 1
    fi
else
    if ! cmake -DCMAKE_BUILD_TYPE=Release "$PROJECT_ROOT"; then
        echo "ERROR: CMake configuration failed!"
        exit 1
    fi
fi

# Build
echo ""
echo "Building..."
NCPU=$(sysctl -n hw.ncpu 2>/dev/null || echo 4)
if ! make -j"$NCPU"; then
    echo "ERROR: Build failed!"
    exit 1
fi

# Verify executable was created
if [ ! -f "bin/VTF-Viewer" ]; then
    echo "ERROR: Executable not found at bin/VTF-Viewer"
    exit 1
fi

# Create app bundle structure
echo ""
echo "Creating app bundle..."
mkdir -p "$RELEASE_DIR/$APP_NAME/Contents/MacOS"
mkdir -p "$RELEASE_DIR/$APP_NAME/Contents/Resources"

# Copy executable
echo "Copying executable..."
cp bin/VTF-Viewer "$RELEASE_DIR/$APP_NAME/Contents/MacOS/"
chmod +x "$RELEASE_DIR/$APP_NAME/Contents/MacOS/VTF-Viewer"

# Copy icon if available
if [ -f "$PROJECT_ROOT/resources/icons/icon.png" ]; then
    echo "Copying icon..."
    cp "$PROJECT_ROOT/resources/icons/icon.png" "$RELEASE_DIR/$APP_NAME/Contents/Resources/"
fi

# Create Info.plist
echo "Creating Info.plist..."
cat > "$RELEASE_DIR/$APP_NAME/Contents/Info.plist" << EOF
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>CFBundleExecutable</key>
    <string>VTF-Viewer</string>
    <key>CFBundleIdentifier</key>
    <string>com.vtfviewer.app</string>
    <key>CFBundleName</key>
    <string>VTF-Viewer</string>
    <key>CFBundleVersion</key>
    <string>$VERSION</string>
    <key>CFBundleShortVersionString</key>
    <string>$VERSION</string>
    <key>CFBundlePackageType</key>
    <string>APPL</string>
    <key>LSMinimumSystemVersion</key>
    <string>10.15</string>
</dict>
</plist>
EOF

# Deploy Qt frameworks
echo ""
echo "Deploying Qt frameworks..."
if ! macdeployqt "$RELEASE_DIR/$APP_NAME"; then
    echo "ERROR: macdeployqt failed!"
    exit 1
fi

# Copy resources
echo ""
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

# Create DMG
cd "$BUILD_DIR"
DMG_NAME="VTF-Viewer-${VERSION}-macOS.dmg"
echo ""
echo "Creating DMG..."
if ! hdiutil create -volname "VTF-Viewer $VERSION" \
                    -srcfolder "$RELEASE_DIR" \
                    -ov -format UDZO \
                    "$DMG_NAME"; then
    echo "ERROR: Failed to create DMG!"
    exit 1
fi

# Verify DMG was created
if [ ! -f "$DMG_NAME" ]; then
    echo "ERROR: DMG not found!"
    exit 1
fi

DMG_SIZE=$(du -h "$DMG_NAME" | cut -f1)

echo ""
echo "==================================="
echo "macOS build complete!"
echo "==================================="
echo "DMG: $BUILD_DIR/$DMG_NAME"
echo "Size: $DMG_SIZE"
echo ""
echo "To test the build:"
echo "  open $RELEASE_DIR/$APP_NAME"
echo "==================================="
