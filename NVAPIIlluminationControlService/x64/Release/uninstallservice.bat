@echo off
cd /d "%~dp0"

set "fileName=NVAPIIlluminationControlService.exe"
set "directory=C:\NvAPIIlluminationController"
del "%directory%\%fileName%"

sc delete "NVAPIIlluminationControlService"