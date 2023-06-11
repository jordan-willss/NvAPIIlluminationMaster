@echo off
cd /d "%~dp0"

set "executable=NVAPIIlluminationControlService.exe"
set "library=NVAPIIlluminationControlLibrary.dll"
set "destinationDirectory=C:\NvAPIIlluminationController"
copy "%executable%" "%destinationDirectory%"
copy "%library%" "%destinationDirectory%"

start "" "NVAPIIlluminationControlService.exe" install