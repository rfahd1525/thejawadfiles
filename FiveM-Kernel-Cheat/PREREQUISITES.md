# Prerequisites & Installation Guide

Complete list of everything you need to download and install before building this project.

## System Requirements

### Operating System
- ✅ **Windows 10** (64-bit) - Version 1809 or newer
- ✅ **Windows 11** (64-bit) - Any version
- ❌ Windows 7/8/8.1 - Not supported
- ❌ 32-bit Windows - Not supported
- ❌ Linux/macOS - Not supported (even with Wine)

### Hardware
- **CPU**: Any modern x64 processor (Intel/AMD)
- **RAM**: 8GB minimum, 16GB recommended
- **Storage**: 5GB free space (for tools + project)
- **Administrator Access**: Required for driver loading

## Required Software

### 1. Visual Studio 2019 or 2022

**Download**: https://visualstudio.microsoft.com/downloads/

**Required Workloads** (during installation):
- ✅ Desktop development with C++
- ✅ Windows 10/11 SDK (latest version)

**Installation Steps**:
1. Download Visual Studio Installer
2. Select "Community" edition (free)
3. In workloads, check "Desktop development with C++"
4. In "Individual Components" tab, ensure these are selected:
   - MSVC v142/v143 - VS 2019/2022 C++ x64/x86 build tools
   - Windows 10/11 SDK (latest)
   - C++ CMake tools for Windows
5. Install (requires ~7-10 GB)

**Verify Installation**:
```cmd
where msbuild
# Should output: C:\Program Files\Microsoft Visual Studio\...\MSBuild.exe
```

---

### 2. Windows Driver Kit (WDK) 10

**Download**: https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk

**Version**: WDK 10.0.22621.0 or newer

**Installation Steps**:
1. First, ensure Visual Studio is installed
2. Download WDK installer from Microsoft
3. Run installer
4. Select default installation path
5. Wait for installation to complete (~2-3 GB)

**Important**: WDK version must match your Windows SDK version

**Verify Installation**:
- Check for: `C:\Program Files (x86)\Windows Kits\10\Include\wdf`
- Open Visual Studio → New Project → should see "Kernel Mode Driver" templates

---

### 3. CMake

**Download**: https://cmake.org/download/

**Version**: 3.15 or newer (latest recommended)

**Installation Steps**:
1. Download Windows x64 Installer
2. Run installer
3. **IMPORTANT**: Check "Add CMake to system PATH for all users"
4. Complete installation

**Verify Installation**:
```cmd
cmake --version
# Should output: cmake version 3.x.x
```

---

### 4. Git

**Download**: https://git-scm.com/download/win

**Installation Steps**:
1. Download Git for Windows
2. Run installer
3. Use default settings
4. Finish installation

**Verify Installation**:
```cmd
git --version
# Should output: git version 2.x.x
```

---

### 5. Dear ImGui (Auto-downloaded)

**Repository**: https://github.com/ocornut/imgui

**Note**: The build script automatically clones this, but you can do it manually:

```cmd
cd FiveM-Kernel-Cheat/client
git clone https://github.com/ocornut/imgui.git
```

---

## Optional but Recommended

### 6. DirectX SDK (Usually included with Windows SDK)

If you encounter DirectX-related errors:

**Download**: https://www.microsoft.com/en-us/download/details.aspx?id=6812

Most modern Windows installations include DirectX 11. You can verify:
```cmd
dxdiag
# Check DirectX Version in System tab
```

---

### 7. Debugging Tools

#### WinDbg (for driver debugging)
**Download**: https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/

#### Process Hacker 2 (for process inspection)
**Download**: https://processhacker.sourceforge.io/

#### Cheat Engine (for finding offsets)
**Download**: https://www.cheatengine.org/

---

## System Configuration

### Enable Test Signing Mode

**Required** for loading unsigned drivers.

**Steps**:
1. Open Command Prompt as **Administrator**
2. Run:
   ```cmd
   bcdedit /set testsigning on
   ```
3. **Reboot your computer** (mandatory!)
4. After reboot, you should see "Test Mode" watermark in bottom-right corner

**To disable later**:
```cmd
bcdedit /set testsigning off
# Then reboot
```

---

### Disable Driver Signature Enforcement (Alternative)

If you don't want permanent test mode:

1. Hold SHIFT and click Restart
2. Navigate: Troubleshoot → Advanced Options → Startup Settings
3. Press Restart
4. Press F7 for "Disable driver signature enforcement"
5. Windows will boot with enforcement disabled (one time only)

**Note**: This is temporary and resets on next reboot.

---

### Disable Secure Boot (If Applicable)

Some systems may require disabling Secure Boot:

1. Enter BIOS/UEFI (usually DEL, F2, or F12 during boot)
2. Find "Secure Boot" option
3. Disable it
4. Save and exit

**Warning**: Only disable if necessary. Research your specific motherboard.

---

## Verify Prerequisites Checklist

Run these commands to verify everything is installed:

```cmd
# Visual Studio
where msbuild

# CMake
cmake --version

# Git
git --version

# Windows SDK (check registry)
reg query "HKLM\SOFTWARE\Microsoft\Windows Kits\Installed Roots"

# Test signing status
bcdedit /enum {current}
# Look for "testsigning" value
```

---

## Download & Extract Project

### Option 1: Git Clone
```cmd
cd C:\Projects
git clone <repository-url> FiveM-Kernel-Cheat
cd FiveM-Kernel-Cheat
```

### Option 2: Download ZIP
1. Download ZIP from repository
2. Extract to `C:\Projects\FiveM-Kernel-Cheat`
3. Open folder

---

## Quick Installation Script

Save this as `check_prerequisites.bat` and run it:

```batch
@echo off
echo ========================================
echo FiveM Kernel Cheat - Prerequisites Check
echo ========================================
echo.

echo [*] Checking Visual Studio...
where msbuild >nul 2>&1
if %errorLevel% == 0 (
    echo [+] Visual Studio: FOUND
) else (
    echo [-] Visual Studio: NOT FOUND
    echo     Download: https://visualstudio.microsoft.com/downloads/
)

echo.
echo [*] Checking CMake...
where cmake >nul 2>&1
if %errorLevel% == 0 (
    echo [+] CMake: FOUND
    cmake --version
) else (
    echo [-] CMake: NOT FOUND
    echo     Download: https://cmake.org/download/
)

echo.
echo [*] Checking Git...
where git >nul 2>&1
if %errorLevel% == 0 (
    echo [+] Git: FOUND
    git --version
) else (
    echo [-] Git: NOT FOUND
    echo     Download: https://git-scm.com/download/win
)

echo.
echo [*] Checking WDK...
if exist "C:\Program Files (x86)\Windows Kits\10\Include\wdf" (
    echo [+] WDK: FOUND
) else (
    echo [-] WDK: NOT FOUND
    echo     Download: https://learn.microsoft.com/en-us/windows-hardware/drivers/download-the-wdk
)

echo.
echo [*] Checking Test Signing...
bcdedit /enum {current} | find "testsigning" | find "Yes" >nul 2>&1
if %errorLevel% == 0 (
    echo [+] Test Signing: ENABLED
) else (
    echo [-] Test Signing: DISABLED
    echo     Run as admin: bcdedit /set testsigning on
    echo     Then reboot!
)

echo.
echo [*] Checking Admin Rights...
net session >nul 2>&1
if %errorLevel% == 0 (
    echo [+] Running as: ADMINISTRATOR
) else (
    echo [!] Running as: USER (Some operations require admin)
)

echo.
echo ========================================
echo Check complete!
echo ========================================
pause
```

---

## Common Issues & Solutions

### "MSBuild not found"
**Solution**: Install Visual Studio with C++ Desktop Development workload

### "CMake not found"
**Solution**: Install CMake and add to PATH during installation

### "Driver failed to load"
**Solution**:
1. Enable test signing: `bcdedit /set testsigning on`
2. Reboot
3. Verify: You should see "Test Mode" watermark

### "WDK templates not showing"
**Solution**:
1. Ensure Visual Studio is installed BEFORE WDK
2. Reinstall WDK if necessary

### "DirectX errors during build"
**Solution**: Install Windows SDK (included with Visual Studio)

### "Git clone fails"
**Solution**: Install Git for Windows from official site

---

## Build Order

Once all prerequisites are installed:

1. **Build Driver**
   ```cmd
   cd driver
   msbuild driver.vcxproj /p:Configuration=Release /p:Platform=x64
   ```

2. **Download ImGui**
   ```cmd
   cd ../client
   git clone https://github.com/ocornut/imgui.git
   ```

3. **Build Client**
   ```cmd
   mkdir build && cd build
   cmake .. -G "Visual Studio 16 2019" -A x64
   cmake --build . --config Release
   ```

4. **Build Loader**
   ```cmd
   cd ../../loader
   mkdir build && cd build
   cmake .. -G "Visual Studio 16 2019" -A x64
   cmake --build . --config Release
   ```

**OR use the automated script**:
```cmd
build.bat
# Select option 4 (Build Everything)
```

---

## Total Download Sizes

- Visual Studio: ~7-10 GB
- WDK: ~2-3 GB
- CMake: ~50 MB
- Git: ~50 MB
- ImGui: ~5 MB

**Total**: ~10-15 GB

**Installation Time**: 30-60 minutes

---

## Quick Start After Prerequisites

1. ✅ All software installed
2. ✅ Test signing enabled + rebooted
3. ✅ Project downloaded/cloned
4. Run `build.bat` → Select option 4
5. Use `loader.exe` to load driver
6. Launch FiveM/GTA V
7. Run `FiveMCheat.exe`

---

## Support

If you're stuck:
1. Check this prerequisites list again
2. Read [QUICKSTART.md](QUICKSTART.md)
3. Read [README.md](README.md)
4. Check Windows Event Viewer for driver errors
5. Verify all prerequisites are installed correctly

---

**Remember**: This is for educational purposes only. Use responsibly.
