# Windows Setup Guide

## Step 1: Install Qt 6

1. Download Qt Online Installer: https://www.qt.io/download-qt-installer
2. Run installer
3. Select Qt 6.5.0 or higher
4. Check "MSVC 2019 64-bit" component
5. Note installation path (e.g., `C:\Qt\6.5.0\msvc2019_64`)

## Step 2: Install Build Tools

### Option A: Visual Studio 2019/2022
1. Download: https://visualstudio.microsoft.com/downloads/
2. Install "Desktop development with C++" workload

### Option B: Build Tools Only
1. Download: https://visualstudio.microsoft.com/downloads/#build-tools-for-visual-studio-2019
2. Install "C++ build tools"

## Step 3: Install CMake

1. Download: https://cmake.org/download/
2. Choose "Windows x64 Installer"
3. During installation, select "Add CMake to system PATH"

## Step 4: Install Ninja

1. Download: https://github.com/ninja-build/ninja/releases
2. Extract `ninja.exe`
3. Add to PATH:
   - Right-click "This PC" → Properties → Advanced System Settings
   - Environment Variables → System variables → Path → Edit
   - Add folder containing `ninja.exe`

## Step 5: Configure Project

1. Open `build.bat` in text editor
2. Update Qt path on line 4:
   ```bat
   set "QT_PATH=C:\Qt\6.5.0\msvc2019_64"
   ```
3. Save file

4. Open `.vscode/settings.json`
5. Update Qt path:
   ```json
   "CMAKE_PREFIX_PATH": "C:/Qt/6.5.0/msvc2019_64"
   ```
6. Save file

## Step 6: Build & Run

### Using Batch Files (Easiest):
```cmd
build.bat
run.bat
```

### Using VS Code:
1. Open folder in VS Code
2. Press `Ctrl+Shift+B` to build
3. Press `F5` to run

### Using Command Line:
```cmd
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2019_64
cmake --build build
build\PulseStation.exe
```

## Troubleshooting

### "Qt6 not found"
- Verify Qt installation path
- Update `CMAKE_PREFIX_PATH` in build scripts

### "ninja not found"
- Add ninja.exe to PATH
- Or use `-G "Visual Studio 16 2019"` instead

### "MSVC not found"
- Open "Developer Command Prompt for VS 2019"
- Run build commands from there

### DLL errors at runtime
- Qt DLLs not in PATH
- Use `run.bat` which sets PATH automatically
- Or copy Qt DLLs to build folder

## Verify Installation

```cmd
cmake --version
ninja --version
```

Both should show version numbers if installed correctly.
