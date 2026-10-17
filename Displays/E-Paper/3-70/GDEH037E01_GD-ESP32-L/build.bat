@echo off
REM ESP-IDF Build Script for Windows
REM This script helps build and flash the GDEH037E01 project

setlocal enabledelayedexpansion

echo =========================================
echo GDEH037E01 EPD Display IDF Build Script
echo =========================================
echo.

REM Check if IDF_PATH is set
if not defined IDF_PATH (
    echo [ERROR] IDF_PATH environment variable not set!
    echo Please set IDF_PATH to your ESP-IDF installation directory.
    echo Example: set IDF_PATH=C:\esp\esp-idf
    pause
    exit /b 1
)

echo [INFO] IDF_PATH: !IDF_PATH!
echo.

REM Parse command line arguments
if "%1"=="" (
    echo Usage: build.bat [command] [options]
    echo.
    echo Commands:
    echo   build         - Build the project
    echo   flash PORT    - Flash to device (e.g., build.bat flash COM3)
    echo   monitor PORT  - Monitor serial output
    echo   clean         - Clean build files
    echo   menuconfig    - Open configuration menu
    echo   buildflash PORT - Build and flash in one command
    echo.
    pause
    exit /b 1
)

set CMD=%1
set PORT=%2

REM Initialize IDF environment
call !IDF_PATH!\tools\idf.py.exe --version >nul 2>&1
if errorlevel 1 (
    echo [ERROR] Failed to initialize ESP-IDF environment!
    pause
    exit /b 1
)

echo [INFO] Building project...
echo.

if /i "!CMD!"=="build" (
    call !IDF_PATH!\tools\idf.py.exe build
    if errorlevel 1 (
        echo [ERROR] Build failed!
        pause
        exit /b 1
    )
    echo [SUCCESS] Build completed!
    echo.
)

if /i "!CMD!"=="flash" (
    if "!PORT!"=="" (
        echo [ERROR] Port not specified!
        echo Usage: build.bat flash COM3
        pause
        exit /b 1
    )
    call !IDF_PATH!\tools\idf.py.exe -p !PORT! flash
    if errorlevel 1 (
        echo [ERROR] Flash failed!
        pause
        exit /b 1
    )
    echo [SUCCESS] Flash completed!
    echo.
)

if /i "!CMD!"=="monitor" (
    if "!PORT!"=="" (
        echo [ERROR] Port not specified!
        echo Usage: build.bat monitor COM3
        pause
        exit /b 1
    )
    call !IDF_PATH!\tools\idf.py.exe -p !PORT! monitor
)

if /i "!CMD!"=="clean" (
    call !IDF_PATH!\tools\idf.py.exe fullclean
    echo [SUCCESS] Clean completed!
    echo.
)

if /i "!CMD!"=="menuconfig" (
    call !IDF_PATH!\tools\idf.py.exe menuconfig
)

if /i "!CMD!"=="buildflash" (
    if "!PORT!"=="" (
        echo [ERROR] Port not specified!
        echo Usage: build.bat buildflash COM3
        pause
        exit /b 1
    )
    echo [INFO] Building...
    call !IDF_PATH!\tools\idf.py.exe build
    if errorlevel 1 (
        echo [ERROR] Build failed!
        pause
        exit /b 1
    )
    echo [INFO] Flashing...
    call !IDF_PATH!\tools\idf.py.exe -p !PORT! flash
    if errorlevel 1 (
        echo [ERROR] Flash failed!
        pause
        exit /b 1
    )
    echo [SUCCESS] Build and flash completed!
    echo.
)

endlocal
