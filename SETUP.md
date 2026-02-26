# PulseStation — Setup & Build Guide

## Windows Setup

### Prerequisites
1. **Qt 5.15**: https://www.qt.io/download-qt-installer
   - Select MinGW 64-bit component
   - Default path: `C:\Qt\5.15.2\mingw81_64`

2. **CMake 3.16+**: https://cmake.org/download/
   - Add to PATH during installation

### Configuration
Edit `build.bat` lines 4–5 with your paths:
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
sudo apt install libqt5bluetooth5-dev
If that fails:
sudo apt install qt5-default qtconnectivity5-dev
```

### Build & Run
```bash
chmod +x build.sh
./build.sh
./build/PulseStation
```

---

## Project Structure

```
PulseStation/
├── main.cpp
├── CMakeLists.txt
├── build.sh
└── src/
    ├── common/               # Shared utilities — edit these to change look & feel
    │   ├── AppColors.h       # ← ALL colors live here
    │   ├── AppTheme.h        # ← ALL styles / font sizes live here
    │   ├── CustomButton.*    # Selectable grid button
    │   ├── PillWidget.*      # Step progress pill
    │   └── SnackBar.*        # Toast notification
    ├── core/                 # Business logic — no UI code
    │   ├── SessionState.*    # User's in-progress session config
    │   ├── DataModels.*      # Static gun/drill data
    │   └── BluetoothManager.*# BLE scanning & connection
    └── ui/
        ├── MainWindow.*      # App shell: header + 3-screen stack
        └── widgets/
            ├── ConsoleWidget.*         # Screen 1: step-by-step configuration
            ├── ReviewScreen.*          # Screen 2: confirm before starting
            ├── TrainingPlaceholder.*   # Screen 3: stub — camera feature goes here
            ├── BluetoothPanel.*        # Left sidebar: BLE device list
            ├── StepFlow.*              # Progress indicator bar
            ├── ButtonGrid.*            # Grid of selectable CustomButtons
            ├── SummaryBox.*            # Session detail card
            └── QRPanel.*              # QR / wristband check-in
```

## Adding the Training Feature

When the camera feature is ready, open `TrainingPlaceholder.cpp` and:

1. Replace `m_cameraPlaceholder` with your real `CameraWidget`.
2. Remove the "Coming Soon" label.
3. Wire `BluetoothManager` data signals to your shot-processing pipeline.
4. Rename the class to `TrainingScreen` if desired (update header + CMakeLists).

---

## Troubleshooting

**Windows: DLL errors**
→ Run `build.bat` again (auto-deploys DLLs via windeployqt)

**Windows: Qt5 not found**
→ Verify `QT_PATH` and `MINGW_PATH` in build.bat

**Linux: Qt5 not found**
```bash
sudo apt install qtbase5-dev
```

**Linux: CMake not found**
```bash
sudo apt install cmake
```
