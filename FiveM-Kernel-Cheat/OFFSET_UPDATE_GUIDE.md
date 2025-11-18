# Offset Update Guide

## Understanding Offsets

GTA V and FiveM are constantly updated. Each update can change memory addresses, breaking cheats. This guide explains how to keep your cheat working.

## What Changed in This Build

### ✅ Automatic Offset Scanner Added

The cheat now includes **automatic offset detection**! If the static offsets don't work, it will:

1. **Validate** the current offsets
2. **Automatically scan** for new offsets using pattern matching
3. **Export** found offsets to a file for future use
4. **Continue working** with the newly found offsets

### How It Works

When you run the cheat:

```
[*] Validating static offsets...
[!] WorldPtr validation failed
[!] Using automatic offset scanner...

========================================
  Automatic Offset Scanner
========================================

[*] Searching for: WorldPtr
[*] Scanning for pattern (mask: xxx????xx?xxxx)...
[+] Pattern found at: 0x7FF6A1234567
[+] WorldPtr final address: 0x7FF6A2515CF8
[+] WorldPtr offset: 0x2515CF8

[*] Export scanned offsets to file? (y/n):
```

If you type `y`, it saves the offsets to `auto_offsets.h` which you can copy into [game_data.h](client/game_data.h).

## Current Offset Status

### Static Offsets (Built-in)
**Game Build**: 3258 (June 2024)

These are hardcoded in [client/game_data.h](client/game_data.h):
- WorldPtr: `0x2515CF8`
- ReplayInterface: `0x1F48798`
- ViewPort: `0x1F8A820`
- PedPool: `0x24F8728`
- VehiclePool: `0x24F8730`

### Latest GTA V Builds (as of Jan 2025)
- **Build 3586** (July 2025) - Latest
- **Build 3570** (June 2025)
- **Build 3407** (December 2024)
- **Build 3323** (September 2024)
- **Build 3258** (June 2024) ← Our static offsets

## If Offsets Are Outdated

### Automatic Method (Recommended)

1. Run the cheat normally:
   ```cmd
   FiveMCheat.exe
   ```

2. If offsets fail validation, the scanner runs automatically

3. When prompted to export:
   ```
   [*] Export scanned offsets to file? (y/n): y
   ```

4. Copy values from `auto_offsets.h` to [client/game_data.h](client/game_data.h)

5. Rebuild the client for better performance

### Force Automatic Scan

You can force scanning even if offsets work:

Edit [client/main.cpp](client/main.cpp):
```cpp
// Change this line:
if (!game.Initialize()) {

// To this:
if (!game.Initialize(true)) {  // true = force auto scan
```

Rebuild and run.

### Manual Method (Advanced)

If automatic scanning fails, manually find offsets:

#### Using Cheat Engine

1. **Download Cheat Engine**: https://www.cheatengine.org/

2. **Attach to GTA5.exe**

3. **Find WorldPtr**:
   - Search for your player health (Float, 4 bytes)
   - Get damaged, search for decreased value
   - Heal, search for increased value
   - Repeat until one address remains
   - Right-click → "Find out what accesses this address"
   - Look for instructions like: `mov rax, [GTA5.exe+XXXXXX]`
   - That XXXXXX is the offset!

4. **Find PedPool**:
   - Search for value: `255` (ped count is usually <255)
   - Spawn/kill peds, watch value change
   - Find instruction accessing it: `mov rcx, [GTA5.exe+XXXXXX]`

5. **Find ViewPort**:
   - Search for Float values 0.0 - 1.0 (projection matrix values)
   - Look for 4x4 matrix structure
   - Find pointer to it

#### Using Pattern Scanning (Advanced)

Our cheat includes patterns in [offset_scanner.h](client/offset_scanner.h):

```cpp
// World pointer pattern
{
    "WorldPtr",
    "\x48\x8B\x05\x00\x00\x00\x00\x45\x00\xC0\x48\x8B\x48",
    "xxx????xx?xxxx",
    3,
    true
}
```

**Pattern Explanation**:
- `\x48\x8B\x05` = `mov rax, [rip+offset]`
- `\x00\x00\x00\x00` = Wildcard (any 4 bytes)
- Mask `xxx????xx?xxxx` = Match x, wildcard ?

If patterns fail, update them:

1. **Disassemble GTA5.exe** with IDA Pro or Ghidra
2. **Find assembly** near the data you need
3. **Create unique signature** from bytes
4. **Update pattern** in offset_scanner.h

## Updating Offsets in Code

Once you have new offsets, update [client/game_data.h](client/game_data.h):

```cpp
// Change game version comment
// GTA V / FiveM Offsets
// NOTE: These offsets are for build 3586 (July 2025)

namespace Offsets {
    // Update values
    constexpr uintptr_t WorldPtr = 0xNEW_VALUE_HERE;
    constexpr uintptr_t ReplayInterface = 0xNEW_VALUE_HERE;
    // ... etc
}
```

Then **rebuild the client**:
```cmd
cd client/build
cmake --build . --config Release
```

## Community Resources

### Finding Updated Offsets

1. **GuidedHacking Forum**
   - https://guidedhacking.com/threads/gta-v-cheats-offsets-structs-reversal-information.13882/
   - Community shares offsets for new builds

2. **UnknownCheats Forum**
   - https://www.unknowncheats.me/forum/grand-theft-auto-v/
   - Active GTA V modding section

3. **GitHub Repositories**
   - Search for "GTA V offsets" on GitHub
   - Many projects share offset dumps

4. **Discord Servers**
   - FiveM modding communities
   - GTA V hacking/modding servers

### Warning About Sources

- ❌ Don't download random executables
- ✅ Only use offset values (numbers)
- ❌ Be careful of malware in "offset updaters"
- ✅ Verify offsets yourself when possible

## Pattern Signature Updates

If patterns break (very rare), update [offset_scanner.h](client/offset_scanner.h):

### How to Find New Patterns

1. **Open GTA5.exe in x64dbg or IDA**

2. **Search for known strings** near the data:
   - "PLAYER"
   - "VEHICLE"
   - Error messages

3. **Find code accessing the data**:
   ```asm
   mov rax, [GTA5.exe+2515CF8]  ; WorldPtr
   mov rcx, [rax+10A8]           ; PlayerInfo offset
   ```

4. **Extract unique bytes**:
   - `48 8B 05 F8 5C 51 02` = `mov rax, [GTA5.exe+2515CF8]`
   - Make pattern: `\x48\x8B\x05????` (leave offset as wildcard)
   - Make mask: `xxx????`

5. **Update pattern in code**:
   ```cpp
   {
       "WorldPtr",
       "\x48\x8B\x05\x00\x00\x00\x00\x45\x00\xC0",  // New bytes
       "xxx????xx?x",                                 // New mask
       3,                                            // Offset in instruction
       true
   }
   ```

## Testing New Offsets

After updating offsets:

1. **Rebuild**: `cmake --build . --config Release`

2. **Test in single-player first**

3. **Check console output**:
   ```
   [+] Found process with PID: 12345
   [+] Base address: 0x7FF600000000
   [+] Using static offsets (validated)  ← Good!
   ```

4. **Verify features work**:
   - ESP shows entities
   - Health/armor display correct
   - Modifications apply successfully

## Automatic Update (Future)

A possible enhancement is fetching offsets from an online database:

```cpp
// Pseudo-code for future implementation
if (!ValidateOffsets()) {
    std::string json = DownloadOffsets("https://example.com/api/offsets/latest");
    ParseAndApplyOffsets(json);
}
```

This would require:
- Hosting offset database
- JSON parsing library
- Internet connection check
- Fallback to local scan

## Frequency of Updates

GTA V updates happen:
- **Major DLC**: Every 3-6 months (likely to break offsets)
- **Patches**: Monthly (may or may not break offsets)
- **Hotfixes**: Weekly (rarely break offsets)

You'll know offsets are outdated when:
- ❌ ESP shows nothing
- ❌ Health/armor reads as 0
- ❌ Modifications don't work
- ❌ Game crashes when using features

## Quick Reference Card

| Problem | Solution |
|---------|----------|
| ESP not showing | Offsets outdated - run auto scan |
| Game crashes | Check Event Viewer, may be anti-cheat |
| "Validation failed" | Automatic scan will run |
| Scan finds nothing | Update patterns in offset_scanner.h |
| Want to help community | Share found offsets on forums |

## Contributing Offsets

If you find new offsets, share them:

1. Export from auto scanner to `auto_offsets.h`
2. Note the game build version
3. Post in community forums
4. Create pull request (if this is on GitHub)

Include:
- Game build number
- Date found
- All offset values
- Whether patterns worked

---

**Remember**: Offsets change with updates. The automatic scanner keeps your cheat working between updates without manual effort!
