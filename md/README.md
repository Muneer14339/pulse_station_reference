# PulseStation Lane Console

## Prerequisites

1. **Qt 6** - Download from https://www.qt.io/download-qt-installer
   - Install Qt 6.5+ with MSVC 2019 64-bit compiler
   
2. **CMake** - Download from https://cmake.org/download/
   - Version 3.16 or higher
   - Add to PATH during installation

3. **Ninja Build System** - Download from https://github.com/ninja-build/ninja/releases
   - Extract and add to PATH

4. **Visual Studio 2019+** - Download from https://visualstudio.microsoft.com/
   - Install "Desktop development with C++" workload
   - Or install MSVC Build Tools separately

5. **VS Code** with extensions:
   - C/C++ (Microsoft)
   - CMake Tools (Microsoft)

## Setup

1. Open VS Code
2. File → Open Folder → Select `PulseStation` folder
3. Press `Ctrl+Shift+P` → "CMake: Select a Kit" → Choose MSVC compiler
4. Set Qt6 environment variable:
   ```cmd
   set CMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2019_64
   ```
   Replace path with your Qt installation

## Build & Run

### Method 1: VS Code Tasks
1. Press `Ctrl+Shift+B` → Build
2. Press `F5` → Run with debugging

### Method 2: Terminal
```cmd
cmake -S . -B build -G Ninja -DCMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2019_64
cmake --build build
.\build\PulseStation.exe
```

### Method 3: Command Prompt
```cmd
mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64 -DCMAKE_PREFIX_PATH=C:\Qt\6.5.0\msvc2019_64
cmake --build . --config Debug
Debug\PulseStation.exe
```

## Project Structure

```
PulseStation/
├── src/
│   ├── common/          # Shared components
│   │   ├── AppColors.h
│   │   ├── AppTheme.h
│   │   ├── CustomButton.cpp/h
│   │   └── PillWidget.cpp/h
│   ├── core/            # Business logic
│   │   ├── SessionState.cpp/h
│   │   └── DataModels.cpp/h
│   └── ui/              # UI components
│       ├── MainWindow.cpp/h
│       └── widgets/
│           ├── ConsoleWidget.cpp/h
│           ├── QRPanel.cpp/h
│           ├── StepFlow.cpp/h
│           ├── ButtonGrid.cpp/h
│           └── SummaryBox.cpp/h
├── CMakeLists.txt
└── main.cpp
```

## Clean Architecture

- **common/**: Reusable UI components, themes, colors
- **core/**: State management and data models
- **ui/**: Screen and widget implementations
- Separation of concerns
- Single responsibility principle
- Easy to maintain and extend
