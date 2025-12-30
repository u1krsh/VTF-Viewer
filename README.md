# VTF-Viewer

<div align="center">

![VTF-Viewer](https://img.shields.io/badge/version-1.0.0-blue.svg)
![License](https://img.shields.io/badge/license-GPL--3.0-green.svg)
![Platform](https://img.shields.io/badge/platform-Linux%20%7C%20Windows%20%7C%20macOS-lightgrey.svg)
![C++](https://img.shields.io/badge/C%2B%2B-17-00599C.svg)
![Qt](https://img.shields.io/badge/Qt-6.0%2B-41CD52.svg)

**A powerful, cross-platform VTF viewer and VMT editor for Source Engine games - Half-Life 2, Team Fortress 2, CS:GO, Portal, Left 4 Dead**

[Features](#features) • [Installation](#building) • [Usage](#usage) • [Contributing](#contributing) • [License](#license)

</div>

---

## Overview

VTF-Viewer is a professional VTF texture viewer and VMT material editor for Source Engine games including Half-Life 2, Counter-Strike: Global Offensive (CS:GO), Team Fortress 2 (TF2), Portal, Portal 2, Left 4 Dead, and Garry's Mod. This cross-platform tool enables game modders, level designers, texture artists, and Source Engine developers to view, analyze, and export Valve Texture Format (VTF) files and Valve Material Type (VMT) files.

Built with C++ and Qt6, VTF-Viewer supports DXT compression, mipmaps, normal maps, specular maps, and animated textures. Whether you're creating custom skins, extracting game assets, converting textures to PNG/JPEG/BMP/TGA formats, or developing Source Engine mods, VTF-Viewer provides a modern interface with batch processing capabilities for efficient texture workflow management.

### Supported Source Engine Games
- Half-Life 2 and Episodes
- Counter-Strike: Source and CS:GO
- Team Fortress 2 (TF2)
- Portal and Portal 2
- Left 4 Dead and Left 4 Dead 2
- Garry's Mod
- Day of Defeat: Source
- Alien Swarm
- Black Mesa
- And other Source Engine games

![VTF-Viewer Interface](resources/screenshot.png)
*VTF-Viewer showing texture gallery, preview, and properties panel*

## Features

### VTF Texture Viewing
- **Comprehensive Format Support**:
  - DXT1, DXT3, DXT5 compression formats
  - RGBA8888, BGRA8888, RGB888, BGR888 uncompressed formats
  - Mipmaps with level-by-level viewing
  - Animated textures with frame navigation
  - Cube maps and volumetric textures

### VMT Material Parsing
- Parse and display VMT material properties
- Shader type identification
- Material parameter extraction
- Texture reference resolution
- Hierarchical property display

### User Interface
- **Gallery View**: Browse textures in an organized thumbnail grid
- **Advanced Image Viewer**:
  - Smooth zoom in/out with mouse wheel support
  - Pan and navigate large textures
  - Fit to window for optimal viewing
  - Full-resolution display
  - Keyboard shortcuts for quick navigation

### Export Functionality
- **Multiple Format Support**: PNG, JPEG, BMP, TGA
- **Batch Export**: Export all loaded textures in one operation
- **Quality Control**: Adjustable JPEG quality settings
- **Preserve Metadata**: Maintain texture properties during export

### Performance
- Efficient memory management for large texture sets
- Multi-threaded texture loading
- Responsive UI even with hundreds of textures

## Building

### Prerequisites

Before building VTF-Viewer, ensure you have the following installed:

- **CMake** 3.16 or higher
- **Qt** 6.0 or higher
- **C++17 compatible compiler**:
  - GCC 7+ (Linux)
  - Clang 5+ (macOS/Linux)
  - MSVC 2017+ (Windows)

### Linux

#### Ubuntu/Debian

```bash
# Install dependencies
sudo apt-get update
sudo apt-get install build-essential cmake qt6-base-dev qt6-base-dev-tools

# Clone the repository (if not already done)
git clone <repository-url>
cd VTF-Viewer

# Build the project
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)

# Run the application
./bin/VTF-Viewer
```

#### Fedora/RHEL

```bash
# Install dependencies
sudo dnf install gcc-c++ cmake qt6-qtbase-devel

# Build (same as above)
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j$(nproc)
./bin/VTF-Viewer
```

### Windows

```cmd
REM Install Qt from https://www.qt.io/download
REM Install CMake from https://cmake.org/download/

REM Open Qt Creator and load CMakeLists.txt
REM Or use command line:
mkdir build
cd build
cmake -G "Visual Studio 16 2019" ..
cmake --build . --config Release

REM Run the application
bin\Release\VTF-Viewer.exe
```

### macOS

```bash
# Install dependencies using Homebrew
brew install cmake qt@6

# Build the project
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH=$(brew --prefix qt@6) ..
make -j$(sysctl -n hw.ncpu)

# Run the application
./bin/VTF-Viewer
```

### Build Options

```bash
# Debug build with symbols
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build with optimizations
cmake -DCMAKE_BUILD_TYPE=Release ..

# Specify Qt installation path
cmake -DCMAKE_PREFIX_PATH=/path/to/qt ..
```

## Usage

### Opening Textures

1. **Launch VTF-Viewer**
2. Click **File → Open Directory** or press `Ctrl+O`
3. Select a folder containing VTF/VMT files
4. Textures will appear in the gallery view

### Navigation

- **Gallery View**: Click any thumbnail to view the full texture
- **Scroll**: Use mouse wheel or scrollbar to browse textures
- **Search**: Use the search bar to filter textures by name

### Viewing Controls

| Action | Keyboard Shortcut | Mouse/Toolbar |
|--------|------------------|---------------|
| Zoom In | `Ctrl++` | Toolbar button / Mouse wheel up |
| Zoom Out | `Ctrl+-` | Toolbar button / Mouse wheel down |
| Reset Zoom | `Ctrl+0` | Toolbar button |
| Fit to Window | `Ctrl+F` | Toolbar button |
| Pan Image | - | Click and drag |

### Exporting

#### Single Texture Export
1. Select a texture in the gallery or viewer
2. Press `Ctrl+E` or click **File → Export Current**
3. Choose format (PNG, JPEG, BMP, TGA) and destination
4. Click **Save**

#### Batch Export
1. Click **File → Export All**
2. Select output format and destination folder
3. Configure quality settings (for JPEG)
4. Click **Export** to process all textures

### Properties Panel

The properties panel displays detailed information about the selected texture:
- **Dimensions**: Width × Height
- **Format**: Compression and color format
- **Mipmap Levels**: Number of mipmap levels
- **Frames**: Animation frame count
- **Flags**: Texture flags and properties
- **VMT Data**: Associated material properties (if available)

## Project Structure

```
VTF-Viewer/
├── CMakeLists.txt           # CMake build configuration
├── README.md                # This file
├── LICENSE                  # GNU GPL v3 license
├── build.sh                 # Quick build script
├── lib/
│   └── VTFLib/              # VTF/VMT parsing library
│       ├── VTFFormat.h      # VTF format definitions and constants
│       ├── VTFFile.h/cpp    # VTF file reader and decoder
│       ├── VMTFile.h/cpp    # VMT material file parser
│       └── VTFLib.h/cpp     # Library initialization and utilities
├── src/
│   ├── main.cpp             # Application entry point
│   ├── MainWindow.h/cpp     # Main application window and menu
│   ├── VTFReader.h/cpp      # Qt wrapper for VTF reading
│   ├── VMTParser.h/cpp      # Qt wrapper for VMT parsing
│   ├── GalleryView.h/cpp    # Thumbnail gallery widget
│   ├── ImageViewer.h/cpp    # Image viewer with zoom/pan
│   ├── PropertiesPanel.h/cpp # Properties display panel
│   └── ExportDialog.h/cpp   # Export configuration dialog
└── resources/
    ├── resources.qrc        # Qt resource file
    └── icons/               # Application icons and assets
```

## Technical Details

### VTF Format Support

VTF-Viewer includes a custom VTFLib implementation with:
- **Version Support**: VTF versions 7.0 through 7.5
- **Decompression**: Hardware-accelerated DXT1/DXT3/DXT5 decompression
- **Format Conversion**: Automatic conversion to RGBA8888 for display
- **Mipmap Extraction**: Access to all mipmap levels
- **Animation Support**: Frame-by-frame access for animated textures
- **Memory Efficiency**: Lazy loading and caching strategies

### VMT Parsing

The VMT parser features:
- **Hierarchical Parsing**: Node-based system for nested properties
- **Shader Detection**: Automatic shader type identification
- **Parameter Extraction**: Complete material parameter parsing
- **Case Insensitivity**: Robust parameter lookup
- **Error Handling**: Graceful handling of malformed files

### Architecture

- **Model-View Architecture**: Clean separation of data and presentation
- **Signal-Slot Communication**: Qt's signal-slot mechanism for loose coupling
- **Resource Management**: RAII principles for automatic cleanup
- **Thread Safety**: Thread-safe texture loading and processing

## Contributing

Contributions are welcome and appreciated! Here's how you can help:

### Reporting Bugs

1. Check if the issue already exists in the issue tracker
2. Create a new issue with:
   - Clear description of the problem
   - Steps to reproduce
   - Expected vs actual behavior
   - System information (OS, Qt version, etc.)
   - Sample VTF/VMT files (if applicable)

### Suggesting Features

1. Open an issue with the `enhancement` label
2. Describe the feature and its use case
3. Explain how it would benefit users

### Submitting Pull Requests

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Make your changes following the coding style
4. Add tests if applicable
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

### Coding Guidelines

- Follow C++17 best practices
- Use Qt naming conventions for Qt-related code
- Add comments for complex logic
- Keep functions focused and concise
- Write meaningful commit messages

## License

This project is licensed under the **GNU General Public License v3.0** - see the [LICENSE](LICENSE) file for details.

### What this means:
- You can use this software for any purpose
- You can modify the source code
- You can distribute the software
- You can distribute your modifications
- You must disclose the source code when distributing
- You must license derivative works under GPL-3.0
- You must include the original copyright notice

## Acknowledgments

- **Valve Software** - For creating the Source Engine and VTF/VMT formats
- **Valve Developer Community** - For comprehensive format documentation
- **Qt Project** - For the excellent cross-platform framework
- **VTFEdit** - Inspiration for features and UI design
- **Source Engine Modding Community** - For feedback and support

## Support

- **Issues**: Report bugs or request features via [GitHub Issues](../../issues)
- **Discussions**: Join the conversation in [GitHub Discussions](../../discussions)
- **Documentation**: Check the [Wiki](../../wiki) for detailed guides

## Roadmap

- [ ] Support for additional texture formats (DDS, TGA input)
- [ ] Texture editing capabilities
- [ ] VMT creation and editing
- [ ] Batch conversion tools
- [ ] Plugin system for custom exporters
- [ ] Command-line interface for automation
- [ ] Texture comparison tools

---

<div align="center">

**Made by the Source Engine community**

If you find this project useful, please consider giving it a star!

</div>
