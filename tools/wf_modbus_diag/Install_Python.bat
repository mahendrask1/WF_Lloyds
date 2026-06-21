@echo off
setlocal EnableExtensions
title Install Python for WF Modbus Diagnostic

echo.
echo ============================================================
echo  Install Python 3.12 (required for Modbus Diagnostic GUI)
echo ============================================================
echo.
echo  This uses Windows Package Manager (winget).
echo  Python will be added for GUI + Modbus tool use.
echo.

where winget >nul 2>&1
if errorlevel 1 (
    echo  winget is not available on this PC.
    echo.
    echo  Install manually from: https://www.python.org/downloads/
    echo  Enable: Add to PATH + tcl/tk and IDLE
    echo.
    pause
    exit /b 1
)

echo  Installing Python.Python.3.12 ...
echo  (Admin prompt may appear — click Yes if asked)
echo.

winget install --id Python.Python.3.12 -e --source winget --accept-package-agreements --accept-source-agreements

if errorlevel 1 (
    echo.
    echo  winget install failed. Try manual install:
    echo  https://www.python.org/downloads/
    echo.
    pause
    exit /b 1
)

echo.
echo ============================================================
echo  Python install finished.
echo ============================================================
echo.
echo  IMPORTANT — disable fake Store python alias:
echo    Settings - Apps - Advanced app settings - App execution aliases
echo    Turn OFF: python.exe  and  python3.exe
echo.
echo  Close and re-open any Command Prompt, then run Find_Python.bat
echo  or double-click desktop shortcut "WF Modbus Diag".
echo.

set "DESKTOP=%USERPROFILE%\Desktop"
if not exist "%DESKTOP%" set "DESKTOP=%USERPROFILE%\OneDrive\Desktop"
set "LNK=%DESKTOP%\WF Modbus Diag.lnk"
if exist "%LNK%" (
    echo  Launching Modbus Diagnostic in 3 seconds...
    timeout /t 3 /nobreak >nul
    start "" "%LNK%"
) else (
    echo  Run WF_Modbus_Diag.bat from:
    echo  %~dp0
)

pause
exit /b 0
