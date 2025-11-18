# FiveM Kernel-Level Cheat

A kernel-mode cheat for FiveM/GTA V with ESP, aimbot, and various gameplay modifications. Built for educational purposes to demonstrate kernel driver development and game hacking techniques.

## ⚠️ Educational Purpose Only

This project is for educational and research purposes only. Using this in online multiplayer games violates Terms of Service and can result in permanent bans. The author is not responsible for any misuse.

## Features

### ESP (Extra Sensory Perception)
- **Bounding Boxes** - Draw boxes around players and vehicles
- **Skeleton ESP** - Display player bone structures
- **Distance Display** - Show distance to entities
- **Health Bars** - Visual health indicators

### Aimbot
- **FOV-Based Targeting** - Configurable field of view
- **Smooth Aiming** - Adjustable smoothing for natural movement
- **Customizable Aim Key** - Multiple key options
- **Head Bone Targeting** - Precise headshot assistance

### Player Modifications
- **Godmode** - Invincibility
- **Infinite Ammo** - Unlimited ammunition
- **No Recoil** - Eliminate weapon recoil and spread
- **Super Speed** - Enhanced movement speed
- **Teleport to Waypoint** - Instant teleportation
- **Clear Wanted Level** - Remove police stars

## Architecture

### Kernel Driver (`driver/`)
- **Direct Memory Access** - Ring 0 memory read/write operations
- **Process Protection** - Anti-debugging and anti-tampering
- **Module Enumeration** - Find game modules via PEB walking
- **IOCTL Interface** - Communication with user-mode client

### User-Mode Client (`client/`)
- **Driver Interface** - High-level wrapper for kernel operations
- **Game Data** - GTA V/FiveM structures and offsets (updated for build 3258)
- **Pattern Scanning** - Dynamic pointer resolution
- **DirectX 11 Overlay** - Transparent window overlay using ImGui
- **Feature Implementation** - ESP rendering, aimbot logic, game modifications

## Building

### Prerequisites

#### For Kernel Driver:
- Windows Driver Kit (WDK) 10
- Visual Studio 2019 or newer with "Desktop development with C++"
- Windows SDK
- Test signing enabled or kernel debugging setup

#### For Client:
- Visual Studio 2019 or newer
- CMake 3.15+
- DirectX 11 SDK (included with Windows SDK)
- ImGui (clone into `client/imgui/`)

### Build Steps

#### 1. Build Kernel Driver

```bash
# Open Visual Studio Developer Command Prompt
cd driver

# Build using MSBuild
msbuild driver.vcxproj /p:Configuration=Release /p:Platform=x64
```

Alternatively, open `driver/driver.sln` in Visual Studio and build in Release x64 configuration.

#### 2. Setup ImGui

```bash
cd client
git clone https://github.com/ocornut/imgui.git
```

Ensure the ImGui directory contains:
- `imgui.cpp`, `imgui.h`, `imgui_draw.cpp`, etc.
- `backends/imgui_impl_dx11.cpp`
- `backends/imgui_impl_win32.cpp`

#### 3. Build Client

```bash
cd client
mkdir build
cd build
cmake ..
cmake --build . --config Release
```

## Installation & Usage

### 1. Enable Test Signing (Required for unsigned drivers)

Open an Administrator Command Prompt:

```cmd
bcdedit /set testsigning on
```

Reboot your system.

### 2. Load the Kernel Driver

```cmd
# Create service
sc create FiveMKernel binPath= "C:\path\to\driver.sys" type= kernel

# Start driver
sc start FiveMKernel

# Verify it's running
sc query FiveMKernel
```

### 3. Run FiveM/GTA V

Launch the game and wait until you're in-game.

### 4. Run the Client

```cmd
FiveMCheat.exe
```

The overlay will appear on your screen. Press **INSERT** to toggle the menu.

### 5. Unload Driver (When Done)

```cmd
sc stop FiveMKernel
sc delete FiveMKernel
```

## Controls

- **INSERT** - Toggle menu visibility
- **Right Mouse Button** (default) - Activate aimbot (configurable in menu)

## Menu Sections

### ESP
- Enable/disable ESP
- Toggle boxes, skeleton, distance, health bar
- All ESP elements are customizable

### Aimbot
- Enable/disable aimbot
- Adjust FOV (10-180 degrees)
- Adjust smoothing (1-20)
- Change aim key binding

### Player
- Toggle godmode
- Toggle infinite ammo
- Toggle no recoil
- Toggle super speed
- Quick actions: heal, clear wanted, teleport

## Offsets

Game offsets are located in `client/game_data.h`. These are updated for GTA V build **3258** (latest as of the project date).

If the game updates, you'll need to:
1. Find new patterns using a signature scanner (Cheat Engine, Reclass.NET, etc.)
2. Update offsets in `game_data.h`
3. Rebuild the client

### Common Offset Categories:
- World pointer
- Player info
- Entity data (health, position, etc.)
- Weapon manager
- Vehicle/Ped pools
- Bone IDs

## Technical Details

### Memory Reading
The kernel driver uses `KeStackAttachProcess` to attach to the target process and read/write memory directly from kernel space, bypassing user-mode protections.

### Pattern Scanning
Dynamic pattern scanning is implemented to find addresses that may change between game sessions or updates.

### ESP Rendering
A DirectX 11 overlay creates a transparent window over the game, using WorldToScreen matrix math to project 3D positions onto 2D screen space.

### Aimbot
The aimbot calculates the closest entity to the crosshair within the configured FOV, then smoothly moves the mouse toward the target's head bone.

## Troubleshooting

### Driver Won't Load
- Ensure test signing is enabled: `bcdedit /set testsigning on`
- Check driver is signed or disable driver signature enforcement
- Verify WDK is installed correctly
- Check Windows Event Viewer for error details

### Can't Find Process
- Make sure FiveM or GTA5.exe is running
- Run the client as Administrator
- Check process names match in `game.h`

### ESP Not Showing
- Verify game is in focus
- Check offsets are correct for your game version
- Ensure overlay window is topmost

### Aimbot Not Working
- Check the aim key binding
- Verify FOV setting isn't too restrictive
- Ensure entities are within range (300m default)
- Check bone positions are being read correctly

### Game Crashes
- Offsets might be incorrect for your game version
- Try disabling features one at a time to isolate the issue
- Check memory read/write operations aren't accessing invalid addresses

## Development Notes

### Adding New Features

1. **Add game offsets** in `client/game_data.h`
2. **Implement game function** in `client/game.h`
3. **Add UI toggle** in `client/overlay.h` DrawMenu()
4. **Process feature** in `client/overlay.h` Render()

### Updating Offsets

Use tools like:
- **Cheat Engine** - Pattern scanning and pointer chains
- **Reclass.NET** - Structure dissection
- **x64dbg** - Debugging and disassembly
- **IDA Pro/Ghidra** - Static analysis

### Signature Scanning

Find unique byte patterns that identify code or data:

```cpp
// Example pattern for World pointer
const char* pattern = "\x48\x8B\x05\x00\x00\x00\x00\x48\x8B\x48\x08";
const char* mask = "xxx????xxxx";

uintptr_t address = driver->PatternScan(processId, baseAddress, 0x10000000, pattern, mask);
```

## Project Structure

```
FiveM-Kernel-Cheat/
├── driver/
│   ├── driver.c          # Main driver code
│   └── driver.vcxproj    # Visual Studio project
├── client/
│   ├── main.cpp          # Entry point
│   ├── driver_interface.h # Driver communication
│   ├── game.h            # Game functions
│   ├── game_data.h       # Offsets and structures
│   ├── overlay.h         # Rendering and features
│   ├── CMakeLists.txt    # Build configuration
│   └── imgui/            # ImGui library
├── shared/
│   └── shared.h          # Shared structures (driver/client)
└── README.md
```

## Dependencies

### Kernel Driver:
- Windows Driver Kit (WDK)
- NT Kernel API

### Client:
- DirectX 11
- ImGui
- Windows API

## Security Considerations

This project demonstrates several kernel-level techniques:
- Process memory manipulation from ring 0
- Anti-cheat evasion via kernel access
- Process protection mechanisms
- Driver-level hooking capabilities

**These techniques should only be used for:**
- Educational research
- Security testing with authorization
- Personal single-player modifications
- Understanding anti-cheat systems

## Legal Disclaimer

This software is provided for educational purposes only. The author does not condone or encourage the use of cheats in online multiplayer games. Using this software may:

- Violate game Terms of Service
- Result in permanent account bans
- Be illegal in some jurisdictions
- Harm the gaming experience of others

Use at your own risk. The author assumes no liability for any consequences arising from the use or misuse of this software.

## License

This project is released for educational purposes. Feel free to learn from the code, but please use responsibly.

## Credits

- **ImGui** - Omar Cornut and contributors
- **DirectX 11** - Microsoft
- **GTA V/FiveM Community** - For reverse engineering research

## Contact

For educational inquiries or security research collaboration, please open an issue on the repository.

---

**Remember**: Real skill in gaming comes from practice, not cheats. Use this knowledge responsibly.
