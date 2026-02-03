@echo off
REM VTF-Viewer Release Build Script for Windows
REM This script builds a release version of VTF-Viewer for Windows

setlocal enabledelayedexpansion

echo ===================================
echo VTF-Viewer Windows Release Build
echo ===================================
echo.

REM Get the absolute path to the script directory
set "SCRIPT_DIR=%~dp0"
set "PROJECT_ROOT=%SCRIPT_DIR%"

REM Remove trailing backslash
if "%PROJECT_ROOT:~-1%"=="\" set "PROJECT_ROOT=%PROJECT_ROOT:~0,-1%"

REM Verify we're in the correct directory
if not exist "%PROJECT_ROOT%\CMakeLists.txt" (
    echo ERROR: CMakeLists.txt not found!
    echo This script must be run from the VTF-Viewer project root directory.
    pause
    exit /b 1
)

REM Configuration
set VERSION=1.0.0
set "BUILD_DIR=%PROJECT_ROOT%\build\release\windows"
set "RELEASE_NAME=VTF-Viewer-%VERSION%-Windows"
set "RELEASE_DIR=%BUILD_DIR%\%RELEASE_NAME%"

REM Check for required tools
echo Checking for required tools...
set MISSING_TOOLS=

where cmake >nul 2>&1
if errorlevel 1 set MISSING_TOOLS=!MISSING_TOOLS! cmake

where windeployqt >nul 2>&1
if errorlevel 1 set MISSING_TOOLS=!MISSING_TOOLS! windeployqt

where 7z >nul 2>&1
if errorlevel 1 (
    echo WARNING: 7z not found. Archive creation will be skipped.
    echo Install 7-Zip from: https://www.7-zip.org/
    set NO_7Z=1
)

if not "!MISSING_TOOLS!"=="" (
    echo ERROR: Missing required tools:!MISSING_TOOLS!
    echo.
    echo Please install them and try again:
    echo - CMake: https://cmake.org/download/
    echo - Qt6: Make sure Qt bin directory is in PATH
    pause
    exit /b 1
)

echo All required tools found.
echo.

REM Clean previous builds with confirmation
if exist "%BUILD_DIR%" (
    echo Previous build directory found at: %BUILD_DIR%
    set /p CLEAN="Do you want to clean it? (y/N): "
    if /i "!CLEAN!"=="y" (
        echo Cleaning previous builds...
        REM Safety check: verify the path contains our expected directory structure
        echo %BUILD_DIR% | findstr /C:"\build\release\windows" >nul
        if errorlevel 1 (
            echo ERROR: Build directory path looks suspicious: %BUILD_DIR%
            echo Aborting for safety.
            pause
            exit /b 1
        )
        rmdir /s /q "%BUILD_DIR%"
        echo Cleaned successfully.
    ) else (
        echo Skipping clean. Build may fail if previous build exists.
    )
)

REM Create build directories
echo Creating build directories...
if not exist "%RELEASE_DIR%" mkdir "%RELEASE_DIR%"
if not exist "%BUILD_DIR%\build-temp" mkdir "%BUILD_DIR%\build-temp"

REM Navigate to build directory
cd /d "%BUILD_DIR%\build-temp"

REM Try to auto-detect Qt path
set QT_PATH=
for /d %%i in (C:\Qt\6.*) do (
    if exist "%%i\msvc*" (
        for /d %%j in (%%i\msvc*) do (
            set "QT_PATH=%%j"
            goto :qt_found
        )
    )
)
:qt_found

REM Configure with CMake
echo.
echo Configuring CMake...
if not "!QT_PATH!"=="" (
    echo Using Qt from: !QT_PATH!
    cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="!QT_PATH!" "%PROJECT_ROOT%"
) else (
    echo WARNING: Could not auto-detect Qt path. Using system default.
    cmake -G "Visual Studio 16 2019" -A x64 -DCMAKE_BUILD_TYPE=Release "%PROJECT_ROOT%"
)

if errorlevel 1 (
    echo ERROR: CMake configuration failed!
    echo.
    echo If Qt is not found, you may need to set CMAKE_PREFIX_PATH manually.
    echo Example: -DCMAKE_PREFIX_PATH=C:\Qt\6.4.2\msvc2019_64
    pause
    exit /b 1
)

REM Build
echo.
echo Building...
cmake --build . --config Release

if errorlevel 1 (
    echo ERROR: Build failed!
    pause
    exit /b 1
)

REM Verify executable was created
if not exist "bin\Release\VTF-Viewer.exe" (
    echo ERROR: Executable not found at bin\Release\VTF-Viewer.exe
    pause
    exit /b 1
)

REM Copy executable
echo.
echo Copying executable...
copy /y "bin\Release\VTF-Viewer.exe" "%RELEASE_DIR%\"

if errorlevel 1 (
    echo ERROR: Failed to copy executable!
    pause
    exit /b 1
)

REM Copy Qt dependencies
echo Copying Qt dependencies...
windeployqt "%RELEASE_DIR%\VTF-Viewer.exe"

if errorlevel 1 (
    echo ERROR: windeployqt failed!
    pause
    exit /b 1
)

REM Copy resources
echo.
echo Copying resources...
if exist "%PROJECT_ROOT%\LICENSE" (
    copy /y "%PROJECT_ROOT%\LICENSE" "%RELEASE_DIR%\"
) else (
    echo WARNING: LICENSE file not found
)

if exist "%PROJECT_ROOT%\README.md" (
    copy /y "%PROJECT_ROOT%\README.md" "%RELEASE_DIR%\"
) else (
    echo WARNING: README.md file not found
)

REM Create archive
cd /d "%BUILD_DIR%"
if not defined NO_7Z (
    set "ARCHIVE_NAME=VTF-Viewer-%VERSION%-Windows-x64.zip"
    echo.
    echo Creating archive...
    7z a "!ARCHIVE_NAME!" "%RELEASE_NAME%"
    
    if errorlevel 1 (
        echo ERROR: Failed to create archive!
        pause
        exit /b 1
    )
    
    if not exist "!ARCHIVE_NAME!" (
        echo ERROR: Archive not found!
        pause
        exit /b 1
    )
    
    echo.
    echo ===================================
    echo Windows build complete!
    echo ===================================
    echo Archive: %BUILD_DIR%\!ARCHIVE_NAME!
    for %%A in ("!ARCHIVE_NAME!") do echo Size: %%~zA bytes
) else (
    echo.
    echo ===================================
    echo Windows build complete!
    echo ===================================
    echo Location: %RELEASE_DIR%
    echo.
    echo Note: Archive not created (7-Zip not found)
)

echo.
echo To test the build:
echo   cd "%RELEASE_DIR%"
echo   VTF-Viewer.exe
echo ===================================

pause
