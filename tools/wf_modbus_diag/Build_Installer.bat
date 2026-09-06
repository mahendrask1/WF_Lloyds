@echo off
setlocal EnableExtensions
title Build WF Modbus Diagnostic Setup.exe

set "ROOT=%~dp0"
cd /d "%ROOT%"

echo.
echo ============================================================
echo  WF Modbus Diagnostic — build standalone EXE + Setup installer
echo ============================================================
echo.

set "PYEXE="
call :FindPython
if not defined PYEXE goto :NoPython

echo [1/3] Installing PyInstaller (if needed)...
"%PYEXE%" -m pip install --upgrade pip pyinstaller -q
if errorlevel 1 (
    echo pip install failed.
    pause
    exit /b 1
)

echo [2/3] PyInstaller — dist\WF_Modbus_Diag\ ...
"%PYEXE%" -m PyInstaller --noconfirm --clean "wf_modbus_diag.spec"
if errorlevel 1 (
    echo PyInstaller failed.
    pause
    exit /b 1
)

if not exist "dist\WF_Modbus_Diag\WF_Modbus_Diag.exe" (
    echo Expected dist\WF_Modbus_Diag\WF_Modbus_Diag.exe not found.
    pause
    exit /b 1
)

echo [3/3] Inno Setup — installer\output\WF_Modbus_Diag_Setup.exe ...
set "ISCC="
for %%I in (
    "%ProgramFiles(x86)%\Inno Setup 6\ISCC.exe"
    "%ProgramFiles%\Inno Setup 6\ISCC.exe"
) do (
    if exist %%I set "ISCC=%%~I"
)

if not defined ISCC (
    echo.
    echo ============================================================
    echo  Inno Setup 6 is not installed.
    echo ============================================================
    echo.
    echo  Download free from: https://jrsoftware.org/isinfo.php
    echo  Install, then run this script again.
    echo.
    echo  Standalone app is ready without installer:
    echo    %ROOT%dist\WF_Modbus_Diag\
    echo  Zip that folder for manual deployment.
    echo.
    pause
    exit /b 1
)

"%ISCC%" "installer\WF_Modbus_Diag_Setup.iss"
if errorlevel 1 (
    echo Inno Setup compile failed.
    pause
    exit /b 1
)

echo.
echo ============================================================
echo  SUCCESS
echo ============================================================
echo.
echo  Customer installer:
echo    %ROOT%installer\output\WF_Modbus_Diag_Setup.exe
echo.
echo  Send that single file to the customer. No Python required.
echo.
pause
exit /b 0

:NoPython
echo Python 3 not found. Install from https://www.python.org/downloads/
pause
exit /b 1

:FindPython
for %%P in (
    "%LOCALAPPDATA%\Programs\Python\Python313\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python312\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python311\python.exe"
    "%ProgramFiles%\Python312\python.exe"
) do (
    if exist %%P set "PYEXE=%%~P" & exit /b 0
)
where py >nul 2>&1 && ( set "PYEXE=py" & exit /b 0 )
where python >nul 2>&1 && ( set "PYEXE=python" & exit /b 0 )
exit /b 1
