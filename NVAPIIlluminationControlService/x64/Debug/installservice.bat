@echo off
cd /d "%~dp0"

set "sourceFile=NVAPIIlluminationControlService.exe"
set "destinationDirectory=C:\NvAPIIlluminationController"
copy "%sourceFile%" "%destinationDirectory%"

start "" "NVAPIIlluminationControlService.exe" install