# Architecture Overview

## System Architecture Diagram

```
┌─────────────────────────────────────────────────────────────────┐
│                        USER SPACE (Ring 3)                       │
├─────────────────────────────────────────────────────────────────┤
│                                                                  │
│  ┌────────────────────────────────────────────────────────┐    │
│  │              FiveM Cheat Client (main.exe)             │    │
│  │                                                        │    │
│  │  ┌──────────────┐  ┌──────────────┐  ┌─────────────┐ │    │
│  │  │   Overlay    │  │     Game     │  │   Driver    │ │    │
│  │  │   System     │  │   Interface  │  │  Interface  │ │    │
│  │  │              │  │              │  │             │ │    │
│  │  │  - DirectX11 │  │  - Entity    │  │  - IOCTL    │ │    │
│  │  │  - ImGui     │  │    Pool      │  │  - Read/    │ │    │
│  │  │  - ESP Draw  │  │  - Memory    │  │    Write    │ │    │
│  │  │  - Menu UI   │  │    Access    │  │  - Pattern  │ │    │
│  │  │  - Aimbot    │  │  - Offsets   │  │    Scan     │ │    │
│  │  └──────┬───────┘  └──────┬───────┘  └──────┬──────┘ │    │
│  │         │                 │                  │        │    │
│  │         └────────┬────────┴──────────────────┘        │    │
│  │                  │                                     │    │
│  └──────────────────┼─────────────────────────────────────┘    │
│                     │                                           │
│                     │ DeviceIoControl()                         │
│                     │                                           │
├─────────────────────┼───────────────────────────────────────────┤
│                     │                                           │
│                     ▼                                           │
│            ┌─────────────────┐                                 │
│            │ Symbolic Link   │                                 │
│            │ \\.\\FiveMKernel │                                 │
│            └────────┬────────┘                                 │
│                     │                                           │
└─────────────────────┼───────────────────────────────────────────┘
┌─────────────────────┼───────────────────────────────────────────┐
│                     │      KERNEL SPACE (Ring 0)                │
├─────────────────────┼───────────────────────────────────────────┤
│                     ▼                                           │
│         ┌──────────────────────────┐                           │
│         │   Kernel Driver          │                           │
│         │   (FiveMKernel.sys)      │                           │
│         │                          │                           │
│         │  ┌────────────────────┐  │                           │
│         │  │  IOCTL Dispatcher  │  │                           │
│         │  └─────────┬──────────┘  │                           │
│         │            │              │                           │
│         │    ┌───────┴────────┐    │                           │
│         │    │                │    │                           │
│         │    ▼                ▼    │                           │
│         │ ┌─────────┐  ┌──────────┐│                           │
│         │ │  Read   │  │  Write   ││                           │
│         │ │ Memory  │  │  Memory  ││                           │
│         │ └────┬────┘  └────┬─────┘│                           │
│         │      │            │      │                           │
│         │      │  ┌─────────┴──┐   │                           │
│         │      │  │  Module    │   │                           │
│         │      │  │  Enum      │   │                           │
│         │      │  └─────┬──────┘   │                           │
│         │      │        │          │                           │
│         └──────┼────────┼──────────┘                           │
│                │        │                                       │
│                ▼        ▼                                       │
│      ┌────────────────────────────┐                           │
│      │  KeStackAttachProcess()    │                           │
│      │  PsLookupProcessByProcessId│                           │
│      │  PsGetProcessPeb()         │                           │
│      │  RtlCopyMemory()           │                           │
│      └────────────┬───────────────┘                           │
│                   │                                            │
│                   ▼                                            │
│         ┌─────────────────────┐                               │
│         │  Target Process     │                               │
│         │  (GTA5.exe/FiveM)   │                               │
│         │                     │                               │
│         │  - World Data       │                               │
│         │  - Entity Pools     │                               │
│         │  - Player Info      │                               │
│         │  - Weapon Manager   │                               │
│         └─────────────────────┘                               │
└───────────────────────────────────────────────────────────────┘
```

## Communication Flow

### 1. Memory Read Operation

```
Client Request                   Kernel Driver                Target Process
     │                                │                            │
     │  DeviceIoControl()             │                            │
     │  IOCTL_READ_MEMORY             │                            │
     ├───────────────────────────────►│                            │
     │                                │                            │
     │                                │  PsLookupProcessByProcessId│
     │                                ├───────────────────────────►│
     │                                │                            │
     │                                │  KeStackAttachProcess()    │
     │                                ├───────────────────────────►│
     │                                │                            │
     │                                │  RtlCopyMemory()           │
     │                                ├───────────────────────────►│
     │                                │◄───────────────────────────┤
     │                                │         Data               │
     │                                │                            │
     │                                │  KeUnstackDetachProcess()  │
     │                                ├───────────────────────────►│
     │                                │                            │
     │◄───────────────────────────────┤                            │
     │         Return Data            │                            │
     │                                │                            │
```

### 2. ESP Rendering Flow

```
Main Loop
   │
   ├──► Get ViewMatrix (from game)
   │
   ├──► Enumerate Entities
   │      │
   │      ├──► Read Ped Pool
   │      ├──► Read Vehicle Pool
   │      └──► Filter by distance
   │
   ├──► For each entity:
   │      │
   │      ├──► Get 3D Position
   │      ├──► WorldToScreen()
   │      ├──► Get Health/Armor
   │      ├──► Get Bone Positions
   │      │
   │      └──► Draw:
   │           ├──► Bounding Box
   │           ├──► Skeleton
   │           ├──► Health Bar
   │           └──► Distance Text
   │
   └──► Render to overlay
```

### 3. Aimbot Processing Flow

```
Main Loop (when aim key pressed)
   │
   ├──► Get Local Player Position
   │
   ├──► Get All Entities in Range
   │
   ├──► For each entity:
   │      │
   │      ├──► Get Head Bone Position
   │      ├──► WorldToScreen()
   │      ├──► Calculate distance from crosshair
   │      ├──► Check if within FOV
   │      │
   │      └──► Select closest to crosshair
   │
   ├──► Calculate aim vector
   │
   ├──► Apply smoothing
   │
   └──► Move mouse
```

## Component Responsibilities

### Driver Interface Layer
**Purpose**: Abstract kernel operations for easy use

- Wraps IOCTL calls
- Provides type-safe templates
- Handles errors gracefully
- Implements pattern scanning
- Caches frequently accessed data

### Game Interface Layer
**Purpose**: Abstract game-specific operations

- Manages process/module handles
- Provides high-level game functions
- Encapsulates offset logic
- Enumerates game entities
- Performs calculations

### Overlay Layer
**Purpose**: Visual rendering and UI

- Creates transparent window
- Manages DirectX context
- Renders ESP elements
- Draws ImGui menu
- Processes user input

### Kernel Driver Layer
**Purpose**: Low-level memory access

- Provides ring 0 access
- Bypasses user-mode protections
- Implements safe memory operations
- Handles process attachment
- Manages IOCTL interface

## Data Flow Diagram

```
┌─────────────┐     ┌──────────────┐     ┌─────────────┐
│   Offsets   │────►│ Game Memory  │────►│   Entities  │
│  (Static)   │     │   (Dynamic)  │     │  (Runtime)  │
└─────────────┘     └──────────────┘     └─────────────┘
                            │                    │
                            │                    │
                            ▼                    ▼
                    ┌──────────────┐     ┌─────────────┐
                    │ World State  │     │   Overlay   │
                    │              │     │  Rendering  │
                    └──────────────┘     └─────────────┘
                            │                    │
                            │                    │
                            ▼                    ▼
                    ┌──────────────┐     ┌─────────────┐
                    │  Features    │     │    User     │
                    │ (Mods/Hacks) │     │  Interface  │
                    └──────────────┘     └─────────────┘
```

## Memory Layout

```
GTA5.exe Process Memory Space
┌──────────────────────────────────────┐
│ Module Base (GTA5.exe)               │  ◄─── Retrieved by driver
│  0x7FF600000000 (example)            │
├──────────────────────────────────────┤
│                                      │
│  .text (code section)                │
│  ┌────────────────────────────────┐  │
│  │  World Pointer: Base + 0x251... │  │  ◄─── Static offsets
│  │  Ped Pool: Base + 0x24F...      │  │
│  │  Vehicle Pool: Base + 0x24F...  │  │
│  └────────────────────────────────┘  │
│                                      │
├──────────────────────────────────────┤
│                                      │
│  .data (data section)                │
│  ┌────────────────────────────────┐  │
│  │  World Instance                │  │  ◄─── Dereferenced pointers
│  │  ├─ Player Info                │  │
│  │  │  └─ Player Ped ───────┐     │  │
│  │  │                       │     │  │
│  │  └─ Entity Pools         │     │  │
│  │     ├─ Ped Pool ────┐    │     │  │
│  │     └─ Vehicle Pool  │    │     │  │
│  └─────────────────────┼────┼─────┘  │
│                        │    │        │
│  ┌─────────────────────┼────┼─────┐  │
│  │  Entity Objects     │    │     │  │  ◄─── Dynamic game objects
│  │                     ▼    ▼     │  │
│  │  ┌──────────┐  ┌──────────┐   │  │
│  │  │  Player  │  │   NPC    │   │  │
│  │  │  Entity  │  │  Entity  │   │  │
│  │  │          │  │          │   │  │
│  │  │ +0x90: Position        │   │  │  ◄─── Entity offsets
│  │  │ +0x280: Health         │   │  │
│  │  │ +0x430: Bones          │   │  │
│  │  └──────────┘  └──────────┘   │  │
│  └────────────────────────────────┘  │
│                                      │
└──────────────────────────────────────┘
```

## Thread Model

```
┌──────────────────────────────────────────────────┐
│               Client Application                  │
├──────────────────────────────────────────────────┤
│                                                   │
│  Main Thread                                      │
│  ├─ Overlay Rendering (60 FPS)                   │
│  ├─ ImGui Frame Processing                       │
│  ├─ Input Handling                               │
│  └─ Feature Updates                              │
│                                                   │
│  Background Tasks (potential)                     │
│  ├─ Entity Caching                               │
│  ├─ Pattern Scanning                             │
│  └─ Offset Verification                          │
│                                                   │
└──────────────────────────────────────────────────┘
```

## Error Handling Strategy

```
Operation Request
      │
      ▼
┌──────────────┐
│  Validation  │  ◄─── Check parameters
└──────┬───────┘
       │
       ▼
┌──────────────┐
│   Execute    │  ◄─── Try operation
└──────┬───────┘
       │
       ├─ Success ──► Return data
       │
       └─ Failure ──┬──► Log error
                    ├──► Return safe default
                    └──► Continue execution
```

## Security Layers

```
Anti-Cheat Detection
        │
        ▼
┌────────────────┐
│ Kernel Driver  │  ◄─── Operates in ring 0
│ (Ring 0)       │       Harder to detect
└────────┬───────┘
         │
         ▼
┌────────────────┐
│ Process        │  ◄─── Protected from debugging
│ Protection     │
└────────┬───────┘
         │
         ▼
┌────────────────┐
│ Memory Access  │  ◄─── Direct kernel-level access
│ (Bypass        │       Bypasses hooks
│  User-Mode)    │
└────────────────┘
```

## Performance Optimization

### Caching Strategy
```
First Access          Subsequent Access
     │                      │
     ▼                      ▼
Read from ────►  ┌──────────────┐  ◄──── Read from
Game Memory      │    Cache     │        Cache
(Slow)           │  (Fast)      │        (Fast)
                 └──────────────┘
                        │
                        ▼
                  Invalidate on:
                  - Time elapsed
                  - Value changed
                  - Manual refresh
```

### Render Optimization
```
Frame Start
     │
     ├──► Frustum Culling (if behind camera, skip)
     │
     ├──► Distance Culling (if too far, skip)
     │
     ├──► Batch Similar Draw Calls
     │
     ├──► Minimize State Changes
     │
     └──► V-Sync to 60 FPS
```

## Build Pipeline

```
Source Files
     │
     ├──── Driver (.c) ────► MSBuild ────► driver.sys
     │
     ├──── Client (.cpp) ──► CMake ──┬──► FiveMCheat.exe
     │                                │
     └──── Loader (.cpp) ──► CMake ──┴──► loader.exe
```

## Runtime Initialization Sequence

```
1. Loader.exe
   ├─ Create Service
   ├─ Start Driver
   └─ Verify Loaded

2. FiveMCheat.exe
   ├─ Connect to Driver
   ├─ Find Game Process
   ├─ Get Module Base
   ├─ Initialize Overlay
   ├─ Setup ImGui
   └─ Enter Main Loop

3. Main Loop
   ├─ Process Input
   ├─ Update Features
   ├─ Render ESP
   ├─ Render Menu
   └─ Present Frame
```

## Shutdown Sequence

```
User Closes Application
         │
         ▼
   Cleanup Overlay
         │
         ▼
   Release DirectX
         │
         ▼
   Close Driver Handle
         │
         ▼
 (Optional) Unload Driver
         │
         ▼
      Exit
```

---

This architecture provides:
- **Separation of Concerns**: Each layer has a specific responsibility
- **Modularity**: Components can be updated independently
- **Security**: Kernel-level access with user-mode control
- **Performance**: Optimized rendering and memory access
- **Maintainability**: Clear structure and documentation
