# ValveExplorer

A C++ application with Qt UI for viewing and exporting Source Engine VTF (Valve Texture Format) and VMT (Valve Material Type) files.

![ValveExplorer](https://img.shields.io/badge/version-1.0.0-blue.svg)
![License](https://img.shields.io/badge/license-MIT-green.svg)

## Features

- **VTF Texture Viewing**: Load and view Source Engine VTF textures with full support for:
  - DXT1, DXT3, DXT5 compression formats
  - RGBA8888, BGRA8888, RGB888, BGR888 formats
  - Mipmaps and animated textures
  - Cube maps and volumetric textures

- **VMT Material Parsing**: Parse and display VMT material properties including:
  - Shader types
  - Material parameters
  - Texture references

- **Gallery View**: Browse textures in a thumbnail grid layout

- **Image Viewer**: 
  - Zoom in/out
  - Pan support
  - Fit to window
  - Full-resolution viewing

- **Export Functionality**:
  - Export to PNG, JPEG, BMP, TGA formats
  - Batch export all loaded textures
  - Quality settings for JPEG export

## Building

### Prerequisites

- CMake 3.16 or higher
- Qt 6.0 or higher
- C++17 compatible compiler (GCC 7+, Clang 5+, MSVC 2017+)

### Linux

```bash
# Install Qt6 development packages
sudo apt-get install qt6-base-dev qt6-base-dev-tools

# Build the project
mkdir build
cd build
cmake ..
make

# Run the application
./bin/ValveExplorer
```

### Build Options

```bash
# Debug build
cmake -DCMAKE_BUILD_TYPE=Debug ..

# Release build
cmake -DCMAKE_BUILD_TYPE=Release ..
```

## Usage

1. **Open Directory**: Click "File" → "Open Directory" or press `Ctrl+O` to select a folder containing VTF/VMT files

2. **View Textures**: Click on any thumbnail in the gallery to view the full texture

3. **Zoom Controls**:
   - Zoom In: `Ctrl++` or toolbar button
   - Zoom Out: `Ctrl+-` or toolbar button
   - Reset Zoom: `Ctrl+0`
   - Fit to Window: `Ctrl+F`

4. **Export**:
   - Export Current: `Ctrl+E` to export the selected texture
   - Export All: Export all loaded textures in batch

5. **Properties Panel**: View detailed information about the selected texture or material

## Project Structure

```
ValveExplorer/
├── CMakeLists.txt
├── README.md
├── lib/
│   └── VTFLib/              # VTF/VMT parsing library
│       ├── VTFFormat.h      # VTF format definitions
│       ├── VTFFile.h/cpp    # VTF file reader
│       ├── VMTFile.h/cpp    # VMT file parser
│       └── VTFLib.h/cpp     # Library initialization
├── src/
│   ├── main.cpp             # Application entry point
│   ├── MainWindow.h/cpp     # Main application window
│   ├── VTFReader.h/cpp      # Qt wrapper for VTF reading
│   ├── VMTParser.h/cpp      # Qt wrapper for VMT parsing
│   ├── GalleryView.h/cpp    # Thumbnail gallery widget
│   ├── ImageViewer.h/cpp    # Image viewer with zoom/pan
│   ├── PropertiesPanel.h/cpp # Properties display panel
│   └── ExportDialog.h/cpp   # Export configuration dialog
└── resources/
    ├── resources.qrc        # Qt resource file
    └── icons/               # Application icons
```

## Technical Details

### VTF Format Support

The application includes a custom VTFLib implementation that supports:
- VTF versions 7.0 through 7.5
- DXT1/DXT5 decompression algorithms
- Multiple image formats with automatic conversion to RGBA8888
- Mipmap level extraction
- Frame-by-frame access for animated textures

### VMT Parsing

The VMT parser uses a hierarchical node-based system to parse material files:
- Shader identification
- Parameter extraction
- Nested property support
- Case-insensitive parameter lookup

## License

This project is licensed under the MIT License.

## Acknowledgments

- VTF/VMT format specifications from Valve Developer Community
- Qt framework for cross-platform GUI development
- Inspired by VTFEdit and other Source Engine modding tools

## Contributing

Contributions are welcome! Please feel free to submit pull requests or open issues for bugs and feature requests.

## Support

For issues, questions, or suggestions, please open an issue on the project repository.
