@echo off
cd /d "%~dp0"

set "fileName=NVAPIIlluminationControlService.exe"
set "destinationDirectory=C:\NvAPIIlluminationController"
del "%directory%\%fileName%"

start "" "NVAPIIlluminationControlService.exe" uninstall