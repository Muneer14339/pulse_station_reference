@echo off
setlocal

set "QT_PATH=C:\Qt\6.10.2\mingw_64"
set "PATH=%QT_PATH%\bin;%PATH%"

if not exist "build\PulseStation.exe" (
    echo PulseStation.exe not found!
    echo Please build the project first using build.bat
    pause
    exit /b 1
)

echo Starting PulseStation...
start "" "build\PulseStation.exe"