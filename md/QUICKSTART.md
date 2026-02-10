# Quick Start Guide

## Prerequisites Setup (One-time)

1. **Install Qt 6.5+**: https://www.qt.io/download-qt-installer
   - Select MSVC 2019 64-bit
   
2. **Install Visual Studio 2019+**: https://visualstudio.microsoft.com/
   - Or just Build Tools with C++ support

3. **Install CMake**: https://cmake.org/download/
   - Add to PATH

4. **Install Ninja**: https://github.com/ninja-build/ninja/releases
   - Extract and add to PATH

## Configuration (One-time)

Edit these files with your Qt path:

**build.bat** line 4:
```bat
set "QT_PATH=C:\Qt\6.5.0\msvc2019_64"
```

**.vscode/settings.json**:
```json
"CMAKE_PREFIX_PATH": "C:/Qt/6.5.0/msvc2019_64"
```

## Build & Run

### Method 1: Batch Files (Easiest)
```cmd
build.bat
run.bat
```

### Method 2: VS Code
1. Open folder in VS Code
2. `Ctrl+Shift+B` to build
3. `F5` to run

### Method 3: Manual
```cmd
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2019_64
cmake --build build
build\PulseStation.exe
```

## First Time Errors?

**"Qt6 not found"** → Check Qt path in scripts
**"ninja not found"** → Add to PATH or use Visual Studio generator
**"DLL missing"** → Use `run.bat` instead of direct exe

Done! The app should launch with dark theme UI matching the HTML design.
