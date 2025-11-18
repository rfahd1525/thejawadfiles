@echo off
setlocal enabledelayedexpansion

echo ========================================
echo FiveM Kernel Cheat Build Script
echo ========================================
echo.

:: Check for admin privileges
net session >nul 2>&1
if %errorLevel% neq 0 (
    echo [!] This script may require Administrator privileges
    echo [!] for some operations. Consider running as Admin.
    echo.
)

:: Check for Visual Studio
where msbuild >nul 2>&1
if %errorLevel% neq 0 (
    echo [X] MSBuild not found!
    echo [!] Please run this from a Visual Studio Developer Command Prompt
    echo [!] or ensure Visual Studio is installed.
    pause
    exit /b 1
)

:: Check for CMake
where cmake >nul 2>&1
if %errorLevel% neq 0 (
    echo [X] CMake not found!
    echo [!] Please install CMake from https://cmake.org/download/
    pause
    exit /b 1
)

echo [*] Found required build tools
echo.

:: Menu
echo Please select what to build:
echo 1. Kernel Driver only
echo 2. Client only
echo 3. Loader only
echo 4. Everything (Driver + Client + Loader)
echo 5. Clean build directories
echo.
set /p choice="Enter your choice (1-5): "

if "%choice%"=="1" goto build_driver
if "%choice%"=="2" goto build_client
if "%choice%"=="3" goto build_loader
if "%choice%"=="4" goto build_all
if "%choice%"=="5" goto clean
goto invalid_choice

:build_driver
echo.
echo ========================================
echo Building Kernel Driver
echo ========================================
cd driver
msbuild driver.vcxproj /p:Configuration=Release /p:Platform=x64
if %errorLevel% neq 0 (
    echo [X] Driver build failed!
    cd ..
    pause
    exit /b 1
)
echo [+] Driver built successfully!
echo [+] Output: driver\x64\Release\driver.sys
cd ..
goto end

:build_client
echo.
echo ========================================
echo Building Client
echo ========================================

:: Check for ImGui
if not exist "client\imgui\imgui.h" (
    echo [!] ImGui not found!
    echo [*] Cloning ImGui repository...
    cd client
    git clone https://github.com/ocornut/imgui.git
    if %errorLevel% neq 0 (
        echo [X] Failed to clone ImGui!
        cd ..
        pause
        exit /b 1
    )
    cd ..
    echo [+] ImGui downloaded successfully
)

cd client
if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
if %errorLevel% neq 0 (
    echo [X] CMake configuration failed!
    cd ..\..
    pause
    exit /b 1
)
cmake --build . --config Release
if %errorLevel% neq 0 (
    echo [X] Client build failed!
    cd ..\..
    pause
    exit /b 1
)
echo [+] Client built successfully!
echo [+] Output: client\build\Release\FiveMCheat.exe
cd ..\..
goto end

:build_loader
echo.
echo ========================================
echo Building Loader
echo ========================================
cd loader
if not exist build mkdir build
cd build
cmake .. -G "Visual Studio 16 2019" -A x64
if %errorLevel% neq 0 (
    echo [X] CMake configuration failed!
    cd ..\..
    pause
    exit /b 1
)
cmake --build . --config Release
if %errorLevel% neq 0 (
    echo [X] Loader build failed!
    cd ..\..
    pause
    exit /b 1
)
echo [+] Loader built successfully!
echo [+] Output: loader\build\Release\loader.exe
cd ..\..
goto end

:build_all
call :build_driver
call :build_client
call :build_loader
goto end

:clean
echo.
echo ========================================
echo Cleaning Build Directories
echo ========================================
if exist "driver\x64" rmdir /s /q "driver\x64"
if exist "driver\Release" rmdir /s /q "driver\Release"
if exist "driver\Debug" rmdir /s /q "driver\Debug"
if exist "client\build" rmdir /s /q "client\build"
if exist "loader\build" rmdir /s /q "loader\build"
echo [+] Build directories cleaned!
goto end

:invalid_choice
echo [X] Invalid choice!
pause
exit /b 1

:end
echo.
echo ========================================
echo Build Complete!
echo ========================================
echo.
echo Next steps:
echo 1. Enable test signing: bcdedit /set testsigning on
echo 2. Reboot your computer
echo 3. Use loader.exe to load the driver
echo 4. Run FiveM/GTA V
echo 5. Run FiveMCheat.exe
echo.
pause
