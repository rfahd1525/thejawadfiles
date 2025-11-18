# Quick Start Guide

Get up and running with FiveM Kernel Cheat in 5 minutes.

## Prerequisites Checklist

- [ ] Windows 10/11 (64-bit)
- [ ] Visual Studio 2019+ with C++ Desktop Development
- [ ] Windows Driver Kit (WDK) 10
- [ ] CMake 3.15+
- [ ] Git
- [ ] Administrator privileges

## Step 1: Build Everything

### Option A: Automated Build (Recommended)

1. Open **Visual Studio Developer Command Prompt** as Administrator
2. Navigate to project directory:
   ```cmd
   cd FiveM-Kernel-Cheat
   ```
3. Run build script:
   ```cmd
   build.bat
   ```
4. Select option **4** (Build Everything)

### Option B: Manual Build

#### Build Driver:
```cmd
cd driver
msbuild driver.vcxproj /p:Configuration=Release /p:Platform=x64
cd ..
```

#### Build Client:
```cmd
cd client
git clone https://github.com/ocornut/imgui.git
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
cd ../..
```

#### Build Loader:
```cmd
cd loader
mkdir build && cd build
cmake .. -G "Visual Studio 16 2019" -A x64
cmake --build . --config Release
cd ../..
```

## Step 2: Enable Test Signing

1. Open Command Prompt as **Administrator**
2. Run:
   ```cmd
   bcdedit /set testsigning on
   ```
3. **Reboot your computer** (required!)

After reboot, you should see "Test Mode" watermark in bottom-right corner.

## Step 3: Load the Driver

1. Open Command Prompt as **Administrator**
2. Navigate to loader directory:
   ```cmd
   cd FiveM-Kernel-Cheat\loader\build\Release
   ```
3. Load driver:
   ```cmd
   loader.exe load ..\..\..\driver\x64\Release\driver.sys
   ```

You should see:
```
[+] Service created successfully
[+] Service started successfully
[+] Driver loaded successfully!
```

## Step 4: Run FiveM/GTA V

Launch FiveM or GTA V and get into the game (past loading screens).

## Step 5: Run the Cheat

1. Navigate to client directory:
   ```cmd
   cd FiveM-Kernel-Cheat\client\build\Release
   ```
2. Run:
   ```cmd
   FiveMCheat.exe
   ```

You should see:
```
[+] Connected to kernel driver
[+] Found process with PID: XXXX
[+] Base address: 0xXXXXXXXX
[+] Overlay initialized
[+] All systems initialized successfully!
```

## Step 6: Use the Cheat

- Press **INSERT** to toggle the menu
- Configure features in the menu
- Enjoy responsibly!

## Troubleshooting

### "Failed to open driver handle"
- Make sure driver is loaded (run loader.exe)
- Check driver is running: `sc query FiveMKernel`

### "Failed to find FiveM/GTA5 process"
- Make sure game is running
- Make sure you're fully in-game, not on loading screen
- Try running as Administrator

### "Driver load failed"
- Did you reboot after enabling test signing?
- Check Event Viewer: Windows Logs → System for errors
- Ensure WDK is installed

### ESP not visible
- Press INSERT to show menu
- Enable ESP options in menu
- Check game is in focus

### Crashes or freezes
- Offsets might be wrong for your game version
- Try disabling features one by one
- Check README for offset update instructions

## Unloading (When Done)

1. Close FiveMCheat.exe
2. Close game
3. Unload driver:
   ```cmd
   loader.exe unload
   ```

To disable test mode (optional):
```cmd
bcdedit /set testsigning off
```
Then reboot.

## Directory Structure After Build

```
FiveM-Kernel-Cheat/
├── driver/x64/Release/
│   └── driver.sys          ← Kernel driver
├── client/build/Release/
│   └── FiveMCheat.exe      ← Main executable
└── loader/build/Release/
    └── loader.exe          ← Driver loader
```

## Common Commands Reference

```cmd
# Enable test signing
bcdedit /set testsigning on

# Load driver
loader.exe load path\to\driver.sys

# Check driver status
sc query FiveMKernel

# Unload driver
loader.exe unload

# Disable test signing
bcdedit /set testsigning off
```

## Feature Overview

| Feature | Default Key | Description |
|---------|-------------|-------------|
| Toggle Menu | INSERT | Show/hide menu |
| Aimbot | Right Mouse | Aim at nearest target in FOV |
| ESP | N/A | See through walls |
| Godmode | Toggle in menu | Invincibility |
| Infinite Ammo | Toggle in menu | Unlimited ammunition |
| No Recoil | Toggle in menu | Remove weapon recoil |
| Super Speed | Toggle in menu | Move faster |

## Safety Tips

1. **Single-player only** - Don't use in online play
2. **Backup saves** - In case something goes wrong
3. **Test in safe area** - Don't immediately join populated servers
4. **Update offsets** - After game updates, offsets need updating
5. **Monitor performance** - If game lags, disable features

## Getting Help

1. Check README.md for detailed documentation
2. Review troubleshooting section
3. Check Windows Event Viewer for driver errors
4. Verify all prerequisites are installed
5. Make sure you're using latest game version offsets

## Performance Tips

- Disable features you're not using
- Lower ESP distance if performance drops
- Close unnecessary background programs
- Use windowed mode for better overlay performance

## Next Steps

- Read full README.md for advanced features
- Learn how to update offsets when game patches
- Explore code to understand implementation
- Consider adding your own features

---

**Remember**: This is for educational purposes only. Use responsibly and ethically.
