@echo off
REM E01 Hash Verification Tool - Windows Build Script
REM This script builds the application using Qt installed from qt.io

echo ========================================
echo E01 Hash Verification Tool - Build
echo ========================================
echo.

REM ===== CONFIGURE THESE PATHS =====
REM Update these paths to match your Qt installation

SET QT_DIR=C:\Qt\6.8.3\mingw_64
SET MINGW_DIR=C:\Qt\Tools\mingw1340_64

REM Verify Qt installation
if not exist "%QT_DIR%\bin\qmake.exe" (
    echo ERROR: Qt not found at %QT_DIR%
    echo.
    echo Please edit build_windows.bat and set QT_DIR to your Qt installation path
    echo Example: C:\Qt\6.8.3\mingw_64
    echo.
    pause
    exit /b 1
)

REM Verify MinGW installation
if not exist "%MINGW_DIR%\bin\g++.exe" (
    echo ERROR: MinGW not found at %MINGW_DIR%
    echo.
    echo Please edit build_windows.bat and set MINGW_DIR to your MinGW installation path
    echo Example: C:\Qt\Tools\mingw1340_64
    echo.
    pause
    exit /b 1
)

echo Qt found at: %QT_DIR%
echo MinGW found at: %MINGW_DIR%
echo.

REM Set up environment
SET PATH=%QT_DIR%\bin;%MINGW_DIR%\bin;%PATH%

REM Create build directory
if not exist build_win (
    mkdir build_win
    echo Created build directory: build_win
)

cd build_win

echo.
echo Running qmake...
qmake.exe ..\e01hasher.pro -spec win32-g++ "CONFIG+=release"

if errorlevel 1 (
    echo.
    echo ERROR: qmake failed!
    cd ..
    pause
    exit /b 1
)

echo.
echo Running make...
mingw32-make.exe -j4

if errorlevel 1 (
    echo.
    echo ERROR: Build failed!
    cd ..
    pause
    exit /b 1
)

cd ..

echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.
echo Executable location: build_win\release\e01hasher.exe
echo.
echo Next steps:
echo 1. Copy Qt DLLs using windeployqt
echo 2. Run: %QT_DIR%\bin\windeployqt.exe build_win\release\e01hasher.exe
echo.
pause
