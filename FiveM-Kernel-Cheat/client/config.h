#pragma once

// Configuration file for easy customization
// Modify these values to change default behavior

namespace Config {
    // Menu
    namespace Menu {
        constexpr int ToggleKey = VK_INSERT;          // Key to toggle menu visibility
        constexpr bool StartVisible = true;           // Menu visible on startup
        constexpr float DefaultWindowWidth = 500.0f;  // Menu window width
        constexpr float DefaultWindowHeight = 600.0f; // Menu window height
    }

    // ESP
    namespace ESP {
        constexpr bool EnabledByDefault = true;       // ESP enabled on startup
        constexpr bool BoxesEnabled = true;           // Draw bounding boxes
        constexpr bool SkeletonEnabled = true;        // Draw skeleton
        constexpr bool DistanceEnabled = true;        // Show distance
        constexpr bool HealthEnabled = true;          // Show health bars
        constexpr float MaxDistance = 500.0f;         // Maximum ESP distance in meters
        constexpr float LineThickness = 2.0f;         // ESP line thickness
        constexpr bool ShowVehicles = true;           // Show vehicles in ESP
        constexpr bool ShowPlayers = true;            // Show players in ESP
    }

    // Aimbot
    namespace Aimbot {
        constexpr bool EnabledByDefault = false;      // Aimbot disabled on startup (safety)
        constexpr float DefaultFOV = 120.0f;          // Field of view in degrees
        constexpr float DefaultSmoothing = 5.0f;      // Aim smoothing (1=instant, 20=very smooth)
        constexpr int DefaultAimKey = VK_RBUTTON;     // Right mouse button
        constexpr bool TargetPlayers = true;          // Target players
        constexpr bool TargetNPCs = false;            // Target NPCs
        constexpr float MaxAimDistance = 300.0f;      // Maximum aimbot distance in meters
        constexpr bool VisibilityCheck = false;       // Only aim at visible targets
    }

    // Player Modifications
    namespace Player {
        constexpr bool GodmodeDefault = false;        // Godmode off by default
        constexpr bool InfiniteAmmoDefault = false;   // Infinite ammo off by default
        constexpr bool NoRecoilDefault = false;       // No recoil off by default
        constexpr bool SuperSpeedDefault = false;     // Super speed off by default
        constexpr float SuperSpeedMultiplier = 2.0f;  // Speed multiplier
        constexpr float HealAmount = 200.0f;          // Health when using heal button
    }

    // Overlay
    namespace Overlay {
        constexpr bool Vsync = true;                  // Enable VSync
        constexpr int RefreshRate = 60;               // Overlay refresh rate
        constexpr float Opacity = 0.95f;              // Window opacity (0.0-1.0)
    }

    // Colors (RGBA 0-255)
    namespace Colors {
        // ESP Colors
        struct Color {
            int r, g, b, a;
        };

        constexpr Color ESPEnemy = {255, 0, 0, 255};           // Red
        constexpr Color ESPAlly = {0, 255, 0, 255};            // Green
        constexpr Color ESPVehicle = {255, 255, 0, 255};       // Yellow
        constexpr Color ESPDead = {100, 100, 100, 255};        // Gray
        constexpr Color HealthBarHigh = {0, 255, 0, 255};      // Green
        constexpr Color HealthBarMid = {255, 255, 0, 255};     // Yellow
        constexpr Color HealthBarLow = {255, 0, 0, 255};       // Red

        // Menu Theme Colors
        constexpr Color MenuAccent = {230, 102, 102, 255};     // Red accent
        constexpr Color MenuBackground = {25, 25, 25, 242};    // Dark background
        constexpr Color MenuText = {255, 255, 255, 255};       // White text
    }

    // Debug
    namespace Debug {
        constexpr bool EnableConsole = true;          // Show console window
        constexpr bool VerboseLogging = false;        // Detailed logging
        constexpr bool ShowFPS = false;               // Show FPS counter
        constexpr bool ShowEntityCount = false;       // Show entity count
    }

    // Advanced
    namespace Advanced {
        constexpr int MaxEntitiesPerFrame = 256;      // Max entities to process per frame
        constexpr int MemoryCacheTime = 100;          // Memory cache time in ms
        constexpr bool UsePatternCaching = true;      // Cache pattern scan results
        constexpr int UpdateRate = 16;                // Feature update rate in ms
    }

    // Safety
    namespace Safety {
        constexpr bool DisableOnlineMode = false;     // Auto-disable if online detected
        constexpr bool RequireConfirmation = false;   // Require confirmation for risky features
        constexpr bool AutoUnloadOnExit = true;       // Unload driver on exit
    }

    // Hotkeys (VK_* virtual key codes)
    namespace Hotkeys {
        constexpr int ToggleESP = VK_F1;              // F1
        constexpr int ToggleAimbot = VK_F2;           // F2
        constexpr int ToggleGodmode = VK_F3;          // F3
        constexpr int TeleportWaypoint = VK_F4;       // F4
        constexpr int QuickHeal = VK_F5;              // F5
        constexpr int PanicKey = VK_END;              // Disable all features
    }
}

// Helper macro for color conversion
#define TO_IMCOLOR(c) IM_COL32(c.r, c.g, c.b, c.a)

// Feature flags (can be toggled at compile time)
#define FEATURE_ESP          1
#define FEATURE_AIMBOT       1
#define FEATURE_GODMODE      1
#define FEATURE_INFINITE_AMMO 1
#define FEATURE_NO_RECOIL    1
#define FEATURE_SUPER_SPEED  1
#define FEATURE_TELEPORT     1

// Platform checks
#ifdef _WIN64
    #define PLATFORM_64BIT 1
#else
    #error "This cheat requires 64-bit Windows"
#endif

// Debug assertions
#ifdef _DEBUG
    #define ASSERT(x) if(!(x)) __debugbreak()
    #define DEBUG_LOG(msg) std::cout << "[DEBUG] " << msg << std::endl
#else
    #define ASSERT(x)
    #define DEBUG_LOG(msg)
#endif
