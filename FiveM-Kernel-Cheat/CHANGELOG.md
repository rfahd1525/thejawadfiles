# Changelog

All notable changes to this project will be documented in this file.

## [1.0.0] - Initial Release

### Added

#### Kernel Driver
- Ring 0 memory read/write operations
- Process attachment via `KeStackAttachProcess`
- IOCTL-based communication interface
- Module base enumeration via PEB walking
- Process protection capabilities
- CR0 manipulation for write protection bypass

#### Client Application
- DirectX 11 overlay with transparency
- ImGui-based menu system
- Driver communication wrapper
- Pattern scanning engine
- Game memory interface
- Entity enumeration and caching

#### ESP Features
- Bounding box ESP for players and vehicles
- Skeleton ESP with full bone structure
- Distance indicators
- Health bar visualization
- Color-coded states (alive/dead)
- Configurable max distance
- Performance-optimized rendering

#### Aimbot
- FOV-based target selection
- Smooth aiming with configurable smoothing
- Multiple aim key options
- Head bone targeting
- Distance-based filtering
- Screen-space calculations
- ViewMatrix integration

#### Player Modifications
- Godmode/invincibility
- Infinite ammunition
- No weapon recoil/spread
- Super speed boost
- Instant waypoint teleportation
- Wanted level manipulation
- Quick heal functionality

#### UI/UX
- Custom ImGui theme
- Keyboard hotkey support
- Real-time feature toggles
- Slider-based configuration
- Organized menu sections
- Console output with status updates

#### Build System
- Visual Studio project for driver
- CMake configuration for client
- CMake configuration for loader
- Automated build script (build.bat)
- Support for VS 2019/2022

#### Tools
- Driver loader utility
- Service creation/deletion
- Driver start/stop management
- Admin privilege elevation
- Detailed error reporting

#### Documentation
- Comprehensive README
- Quick start guide
- Code comments and documentation
- Troubleshooting section
- Offset update instructions
- Architecture overview

#### Configuration
- Centralized config.h file
- Compile-time feature flags
- Customizable colors and settings
- Hotkey configuration
- Debug options

### Game Support
- FiveM (latest)
- GTA V (build 3258)
- 64-bit only

### Technical Details
- Language: C/C++17
- Platform: Windows 10/11 x64
- Graphics API: DirectX 11
- GUI Framework: Dear ImGui
- Architecture: Kernel + User mode

### Known Issues
- Offsets need updating when game patches
- Some anti-cheats may detect kernel driver
- Requires test mode or signed driver
- May not work with certain overlays active

### Security
- Process protection against debugging
- Kernel-level access for anti-cheat evasion
- IOCTL validation
- Exception handling in driver

## Future Considerations

### Potential Features
- [ ] Radar overlay
- [ ] Auto-loot
- [ ] No clip/fly mode
- [ ] Custom waypoint system
- [ ] Entity filters (friends/enemies)
- [ ] Config save/load
- [ ] Multiple aimbot modes (bones)
- [ ] Triggerbot
- [ ] Bhop/bunny hop
- [ ] Silent aim
- [ ] Vehicle modifications
- [ ] Weapon modifications
- [ ] Custom crosshair
- [ ] Spectator list
- [ ] Session information
- [ ] Auto-update offsets from online source

### Technical Improvements
- [ ] Better pattern scanning (AVX2)
- [ ] Multi-threaded entity processing
- [ ] Offset auto-detection
- [ ] Config file persistence
- [ ] Crash handler
- [ ] Memory leak detection
- [ ] Performance profiling
- [ ] Signed driver option
- [ ] HVCI/VBS bypass techniques
- [ ] Alternative communication methods (shared memory)

### Documentation
- [ ] Video tutorials
- [ ] Offset finding guide
- [ ] Driver development guide
- [ ] Anti-cheat analysis
- [ ] Contribution guidelines

## Version History

### v1.0.0 (Initial Release)
- First public release
- Core functionality complete
- Full ESP implementation
- Working aimbot
- Player modifications
- Stable kernel driver
- Complete documentation

---

## Offset Update Log

### Build 3258 (Current)
- WorldPtr: 0x2515CF8
- ReplayInterface: 0x1F48798
- ViewPort: 0x1F8A820
- PedPool: 0x24F8728
- VehiclePool: 0x24F8730
- Last verified: 2024

### How to Update Offsets

When GTA V updates, you'll need to find new offsets:

1. **Use Cheat Engine**
   - Attach to GTA5.exe
   - Search for known values (health, position, etc.)
   - Find pointer chains
   - Calculate offset from base

2. **Use Signature Scanner**
   - Find unique byte patterns near the data
   - Update patterns in code
   - Verify in multiple sessions

3. **Use Reclass.NET**
   - Attach to process
   - Browse structures
   - Identify member offsets
   - Export to C++ headers

4. **Community Resources**
   - Check GTA modding forums
   - UnknownCheats forum
   - GitHub repositories
   - Discord communities

5. **Update game_data.h**
   - Replace old offsets
   - Test in-game
   - Verify all features work
   - Document changes

---

## Contributing

If you want to contribute:
1. Update offsets for new game versions
2. Add new features
3. Improve documentation
4. Report bugs
5. Optimize performance

## License

Educational use only. See LICENSE file for details.
