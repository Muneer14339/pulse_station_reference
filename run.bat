@echo off
setlocal

set "QT_PATH=C:\Qt\5.15.2\mingw81_64"
set "PATH=%QT_PATH%\bin;%PATH%"

if not exist "build\PulseStation.exe" (
    echo PulseStation.exe not found! Run build.bat first.
    pause
    exit /b 1
)

echo Starting PulseStation...
start "" "build\PulseStation.exe"