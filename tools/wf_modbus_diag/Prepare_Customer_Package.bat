@echo off
setlocal EnableExtensions
title Prepare customer package

set "ROOT=%~dp0"
set "SRC=%ROOT%dist\WF_Modbus_Diag"
set "DST=%ROOT%Send_To_Customer\WF_Modbus_Diag"

echo.
echo Building customer folder: Send_To_Customer\
echo.

if not exist "%SRC%\WF_Modbus_Diag.exe" (
    echo dist\WF_Modbus_Diag\WF_Modbus_Diag.exe not found.
    echo Run Build_Installer.bat first ^(PyInstaller step^).
    pause
    exit /b 1
)

if exist "%DST%" rmdir /s /q "%DST%"
mkdir "%DST%" 2>nul

robocopy "%SRC%" "%DST%" /E /NFL /NDL /NJH /NJS /nc /ns /np
if errorlevel 8 (
    echo Copy failed.
    pause
    exit /b 1
)

echo.
echo Done. Send this folder to the customer ^(zip it^):
echo   %ROOT%Send_To_Customer
echo.
echo Contents:
echo   README.txt
echo   Start Modbus Diagnostic.bat
echo   WF_Modbus_Diag\
echo.
pause
