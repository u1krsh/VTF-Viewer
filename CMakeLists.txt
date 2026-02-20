cmake_minimum_required(VERSION 3.16)
project(VTF-Viewer VERSION 1.3.0 LANGUAGES CXX)

# ============================================================================
# Project Configuration
# ============================================================================

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for CLion and other tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# ============================================================================
# Qt6 Configuration
# ============================================================================

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 packages
find_package(Qt6 REQUIRED COMPONENTS Core Widgets Gui)

# ============================================================================
# Compiler Options
# ============================================================================

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(
        -Wall
        -Wextra
        -Wpedantic
        $<$<CONFIG:Debug>:-g>
        $<$<CONFIG:Debug>:-O0>
        $<$<CONFIG:Release>:-O3>
    )
elseif(MSVC)
    add_compile_options(
        /W4
        $<$<CONFIG:Debug>:/Od>
        $<$<CONFIG:Release>:/O2>
    )
endif()

# ============================================================================
# VTFLib - Custom VTF/VMT Parsing Library
# ============================================================================

set(VTFLIB_SOURCES
    lib/VTFLib/VTFFile.cpp
    lib/VTFLib/VMTFile.cpp
    lib/VTFLib/VTFLib.cpp
)

set(VTFLIB_HEADERS
    lib/VTFLib/VTFFile.h
    lib/VTFLib/VMTFile.h
    lib/VTFLib/VTFLib.h
    lib/VTFLib/VTFFormat.h
)

# ============================================================================
# Application Sources
# ============================================================================

set(APP_SOURCES
    src/main.cpp
    src/MainWindow.cpp
    src/VTFReader.cpp
    src/VMTParser.cpp
    src/GalleryView.cpp
    src/ImageViewer.cpp
    src/PropertiesPanel.cpp
    src/ExportDialog.cpp
)

set(APP_HEADERS
    src/MainWindow.h
    src/VTFReader.h
    src/VMTParser.h
    src/GalleryView.h
    src/ImageViewer.h
    src/PropertiesPanel.h
    src/ExportDialog.h
)

# ============================================================================
# Resources
# ============================================================================

set(APP_RESOURCES
    resources/resources.qrc
)

# ============================================================================
# Executable Target
# ============================================================================

add_executable(${PROJECT_NAME}
    ${APP_SOURCES}
    ${APP_HEADERS}
    ${VTFLIB_SOURCES}
    ${VTFLIB_HEADERS}
    ${APP_RESOURCES}
)

# ============================================================================
# Include Directories
# ============================================================================

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/VTFLib
)

# ============================================================================
# Link Libraries
# ============================================================================

target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Widgets
    Qt6::Gui
)

# ============================================================================
# Target Properties
# ============================================================================

set_target_properties(${PROJECT_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin
    # Set working directory for CLion run configurations
    VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
)

# ============================================================================
# IDE Support
# ============================================================================

# Group source files in IDE
source_group("Source Files" FILES ${APP_SOURCES})
source_group("Header Files" FILES ${APP_HEADERS})
source_group("VTFLib\\Source Files" FILES ${VTFLIB_SOURCES})
source_group("VTFLib\\Header Files" FILES ${VTFLIB_HEADERS})
source_group("Resources" FILES ${APP_RESOURCES})

# ============================================================================
# Install Rules
# ============================================================================

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
)

# ============================================================================
# Print Configuration Summary
# ============================================================================

message(STATUS "")
message(STATUS "========================================")
message(STATUS "  VTF-Viewer Configuration Summary")
message(STATUS "========================================")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "C++ Standard: ${CMAKE_CXX_STANDARD}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Qt6 Version: ${Qt6_VERSION}")
message(STATUS "Output directory: ${CMAKE_BINARY_DIR}/bin")
message(STATUS "")
message(STATUS "Features enabled:")
message(STATUS "  - Drag and drop file support")
message(STATUS "  - Recent directories persistence")
message(STATUS "  - Copy to clipboard (Ctrl+C)")
message(STATUS "  - Checkerboard background (B)")
message(STATUS "  - Mouse wheel zoom")
message(STATUS "  - Gallery search/filter (Ctrl+L)")
message(STATUS "  - Gallery sorting (name/size)")
message(STATUS "  - Grid/list view toggle")
message(STATUS "  - Recursive directory scanning")
message(STATUS "  - Reload directory (F5)")
message(STATUS "  - Window state persistence")
message(STATUS "  - Export path persistence")
message(STATUS "  - Adjustable thumbnail sizes")
message(STATUS "  - Image rotation (R / Shift+R)")
message(STATUS "  - Next/Previous navigation (PgUp/PgDn)")
message(STATUS "  - Full screen mode (F11)")
message(STATUS "  - Escape key (exit full screen / clear)")
message(STATUS "  - Image dimensions in status bar")
message(STATUS "  - Current file in title bar")
message(STATUS "  - Detailed gallery tooltips")
message(STATUS "  - Human-readable file sizes")
message(STATUS "========================================")
message(STATUS "")
