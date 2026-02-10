@echo off
setlocal

REM Set Qt path - Change this to your Qt installation
set "QT_PATH=C:\Qt\6.10.2\mingw_64"

set "PATH=%QT_PATH%\bin;C:\Qt\Tools\mingw1310_64\bin;%PATH%"
set "CC=C:\Qt\Tools\mingw1310_64\bin\gcc.exe"
set "CXX=C:\Qt\Tools\mingw1310_64\bin\g++.exe"

REM Add Qt to CMAKE_PREFIX_PATH
set "CMAKE_PREFIX_PATH=%QT_PATH%"

REM Add Qt bin to PATH
set "PATH=%QT_PATH%\bin;%PATH%"

echo === PulseStation Build Script ===
echo.
echo Qt Path: %QT_PATH%
echo.

if not exist build mkdir build

echo Configuring CMake...
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=%CMAKE_PREFIX_PATH% -DCMAKE_C_COMPILER=%CC% -DCMAKE_CXX_COMPILER=%CXX%
if %errorlevel% neq 0 (
    echo CMake configuration failed!
    pause
    exit /b %errorlevel%
)

echo.
echo Building project...
cmake --build build

if %errorlevel% neq 0 (
    echo Build failed!
    pause
    exit /b %errorlevel%
)

echo.
echo === Build Successful! ===
echo Run: build\PulseStation.exe
echo.
pause
