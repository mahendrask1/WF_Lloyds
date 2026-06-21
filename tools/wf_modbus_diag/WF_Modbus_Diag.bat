@echo off
setlocal EnableExtensions
title WF Lloyds Modbus Diagnostic

set "APP_DIR=%~dp0"
cd /d "%APP_DIR%"

if /i "%~1" NEQ "--no-shortcut" call :EnsureDesktopShortcut

set "PYEXE="
call :FindPython
if not defined PYEXE goto :NoPython

echo Using: %PYEXE%

if /i "%PYEXE%"=="py -3" (
    py -3 -c "import tkinter" >nul 2>&1
) else (
    "%PYEXE%" -c "import tkinter" >nul 2>&1
)
if errorlevel 1 goto :NoTkinter

echo Starting WF Modbus Diagnostic GUI...
echo No pip needed - built-in Windows serial support.
if /i "%PYEXE%"=="py -3" (
    py -3 "%APP_DIR%wf_modbus_diag.py"
) else (
    "%PYEXE%" "%APP_DIR%wf_modbus_diag.py"
)
set "RC=%ERRORLEVEL%"
if %RC% NEQ 0 (
    echo.
    echo Application exited with error %RC%.
    pause
)
exit /b %RC%

:NoPython
echo.
echo ============================================================
echo  Python 3 with Tkinter is required but was not found.
echo ============================================================
echo.
echo  The Windows Store "python" stub is NOT sufficient.
echo.
echo  Install from: https://www.python.org/downloads/
echo  During install enable:
echo    [x] Add python.exe to PATH
echo    [x] tcl/tk and IDLE  (needed for GUI)
echo.
echo  Then disable fake aliases:
echo    Settings - Apps - App execution aliases
echo    Turn OFF python.exe and python3.exe
echo.
echo  Or run Install_Python.bat in this folder (one-click winget install).
echo.
pause
exit /b 1

:NoTkinter
echo.
echo Python found but tkinter (GUI) is missing.
echo Re-run Python installer - Modify - tick "tcl/tk and IDLE".
echo.
pause
exit /b 1

:FindPython
for %%P in (
    "%LOCALAPPDATA%\Programs\Python\Python313\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python312\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python311\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python310\python.exe"
    "%ProgramFiles%\Python313\python.exe"
    "%ProgramFiles%\Python312\python.exe"
    "%ProgramFiles%\Python311\python.exe"
    "%ProgramFiles%\Python310\python.exe"
) do (
    if exist %%P call :TestPython "%%~P" & if defined PYEXE exit /b 0
)
where py >nul 2>&1 && ( call :TestPython "py -3" & if defined PYEXE exit /b 0 )
for /f "delims=" %%P in ('where python 2^>nul') do (
    echo %%P | findstr /i "WindowsApps" >nul
    if errorlevel 1 call :TestPython "%%P" & if defined PYEXE exit /b 0
)
exit /b 1

:TestPython
set "CAND=%~1"
if /i "%CAND%"=="py -3" (
    py -3 -c "import sys" >nul 2>&1
) else (
    "%CAND%" -c "import sys" >nul 2>&1
)
if errorlevel 1 exit /b 1
set "PYEXE=%CAND%"
exit /b 0

:EnsureDesktopShortcut
set "DESKTOP=%USERPROFILE%\Desktop"
if not exist "%DESKTOP%" set "DESKTOP=%USERPROFILE%\OneDrive\Desktop"
set "LNK=%DESKTOP%\WF Modbus Diag.lnk"
if exist "%LNK%" exit /b 0
powershell -NoProfile -ExecutionPolicy Bypass -Command "$s=(New-Object -ComObject WScript.Shell).CreateShortcut('%LNK%');$s.TargetPath='%APP_DIR%WF_Modbus_Diag.bat';$s.Arguments='--no-shortcut';$s.WorkingDirectory='%APP_DIR%';$s.Description='WF Lloyds Modbus Diagnostic';$s.IconLocation='%SystemRoot%\System32\imageres.dll,101';$s.Save()" >nul 2>&1
if exist "%LNK%" echo Desktop shortcut created.
exit /b 0
