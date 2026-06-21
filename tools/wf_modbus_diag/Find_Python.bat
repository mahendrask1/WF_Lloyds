@echo off
setlocal EnableExtensions
title Find Python on this PC

echo.
echo ============================================================
echo  Python installations on this PC
echo ============================================================
echo.

echo --- What PATH finds (may be fake Store stub) ---
where python 2>nul || echo   (none)
where py 2>nul || echo   py launcher: not found
echo.

echo --- Scanning common locations ---
set "N=0"

for %%P in (
    "%LOCALAPPDATA%\Programs\Python\Python313\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python312\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python311\python.exe"
    "%LOCALAPPDATA%\Programs\Python\Python310\python.exe"
    "%ProgramFiles%\Python313\python.exe"
    "%ProgramFiles%\Python312\python.exe"
    "%ProgramFiles%\Python311\python.exe"
    "%ProgramFiles%\Python310\python.exe"
    "C:\Espressif\tools\idf-python\3.11.2\python.exe"
    "C:\Espressif\python_env\idf5.5_py3.11_env\Scripts\python.exe"
    "C:\Espressif\python_env\idf5.3_py3.11_env\Scripts\python.exe"
    "C:\Program Files\KiCad\10.0\bin\python.exe"
) do (
    if exist %%P call :CheckOne "%%~P"
)

echo.
echo ============================================================
echo  For WF Modbus Diag GUI you need Python with TKINTER.
echo  Install from https://www.python.org/downloads/
echo  Tick: Add to PATH + tcl/tk and IDLE
echo ============================================================
echo.
pause
exit /b 0

:CheckOne
set /a N+=1
set "EXE=%~1"
echo [%N%] %EXE%
for /f "delims=" %%V in ('"%EXE%" --version 2^>^&1') do echo      Version: %%V
"%EXE%" -c "import tkinter" >nul 2>&1
if errorlevel 1 (echo      tkinter: MISSING - cannot run GUI) else (echo      tkinter: OK - CAN RUN MODBUS DIAG)
"%EXE%" -m pip --version >nul 2>&1
if errorlevel 1 (echo      pip:     not available) else (
    for /f "delims=" %%P in ('"%EXE%" -m pip --version 2^>^&1') do echo      pip:     %%P
)
echo %EXE% | findstr /i "WindowsApps" >nul && echo      NOTE: This is the Windows Store stub - NOT real Python
echo.
exit /b 0
