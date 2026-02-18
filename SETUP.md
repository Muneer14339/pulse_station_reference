# PulseStation - Setup & Build Guide

## Windows Setup

### Prerequisites
1. **Qt 5.15**: https://www.qt.io/download-qt-installer
   - Select MinGW 64-bit component
   - Default path: `C:\Qt\5.15.2\mingw81_64`

2. **CMake 3.16+**: https://cmake.org/download/
   - Add to PATH during installation

### Configuration
Edit `build.bat` lines 4-5 with your paths:
```bat
set "QT_PATH=C:\Qt\5.15.2\mingw81_64"
set "MINGW_PATH=C:\Qt\Tools\mingw810_64"
```

### Build & Run
```cmd
build.bat
build\PulseStation.exe
```

---

## Linux Setup (Ubuntu 20.04 LTS)

### Prerequisites
```bash
sudo apt update
sudo apt install qtbase5-dev cmake g++ make
```

### Build & Run
```bash
chmod +x build.sh
./build.sh
./build/PulseStation
```

---

## Troubleshooting

**Windows: DLL errors when running exe directly**
- Run `build.bat` again (auto-deploys DLLs via windeployqt)

**Windows: Qt5 not found during build**
- Verify QT_PATH and MINGW_PATH in build.bat

**Linux: Qt5 not found**
```bash
sudo apt install qtbase5-dev
```

**Linux: CMake not found**
```bash
sudo apt install cmake
```