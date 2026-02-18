@echo off
setlocal

REM Set Qt5 path - Change this to your Qt5 installation
set "QT_PATH=C:\Qt\5.15.2\mingw81_64"
set "MINGW_PATH=C:\Qt\Tools\mingw810_64"

set "PATH=%QT_PATH%\bin;%MINGW_PATH%\bin;%PATH%"
set "CC=%MINGW_PATH%\bin\gcc.exe"
set "CXX=%MINGW_PATH%\bin\g++.exe"
set "CMAKE_PREFIX_PATH=%QT_PATH%"

echo === PulseStation Build Script (Qt5) ===
echo Qt Path: %QT_PATH%
echo.

if not exist build mkdir build

echo Configuring CMake...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -DCMAKE_C_COMPILER=%CC% -DCMAKE_CXX_COMPILER=%CXX%
if %errorlevel% neq 0 ( echo CMake failed! & pause & exit /b %errorlevel% )

echo Building...
cmake --build build
if %errorlevel% neq 0 ( echo Build failed! & pause & exit /b %errorlevel% )

echo Deploying Qt DLLs...
cd build
%QT_PATH%\bin\windeployqt.exe --release --no-translations PulseStation.exe
cd ..

echo.
echo === Build Successful! ===
echo Run: build\PulseStation.exe
pause