#pragma once
#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

// GTA V / FiveM Offsets
// NOTE: These offsets are for build 3258 (June 2024)
// Latest builds: 3586 (July 2025), 3570 (June 2025), 3407 (Dec 2024)
// If these don't work, the cheat will automatically scan for new offsets!
namespace Offsets {
    // Base pointers
    constexpr uintptr_t WorldPtr = 0x2515CF8;           // World base
    constexpr uintptr_t ReplayInterface = 0x1F48798;    // Replay interface
    constexpr uintptr_t ViewPort = 0x1F8A820;           // Viewport
    constexpr uintptr_t Waypoint = 0x1F92AE0;           // Waypoint coords
    constexpr uintptr_t LocalScripts = 0x1F7F310;       // Local scripts
    constexpr uintptr_t WeaponManager = 0x2529580;      // Weapon manager
    constexpr uintptr_t VehiclePool = 0x24F8730;        // Vehicle pool
    constexpr uintptr_t PedPool = 0x24F8728;            // Ped pool
    constexpr uintptr_t ObjectPool = 0x24F8738;         // Object pool

    // Player offsets
    namespace Player {
        constexpr uintptr_t PlayerInfo = 0x10A8;
        constexpr uintptr_t PlayerPed = 0x08;
        constexpr uintptr_t WantedLevel = 0x888;
        constexpr uintptr_t Godmode = 0x189;
    }

    // Entity offsets
    namespace Entity {
        constexpr uintptr_t EntityType = 0x10B8;
        constexpr uintptr_t Coordinates = 0x90;
        constexpr uintptr_t Health = 0x280;
        constexpr uintptr_t MaxHealth = 0x2A0;
        constexpr uintptr_t Armour = 0x14B0;
        constexpr uintptr_t Velocity = 0x300;
        constexpr uintptr_t VisualHealth = 0x1030;
    }

    // Ped offsets
    namespace Ped {
        constexpr uintptr_t WeaponManager = 0x10D8;
        constexpr uintptr_t BoneManager = 0x430;
        constexpr uintptr_t RegenRate = 0x1058;
        constexpr uintptr_t Ragdoll = 0x10A8;
        constexpr uintptr_t Seatbelt = 0x143C;
        constexpr uintptr_t InVehicle = 0xE52;
        constexpr uintptr_t CurrentVehicle = 0xD30;
    }

    // Vehicle offsets
    namespace Vehicle {
        constexpr uintptr_t Driver = 0xC68;
        constexpr uintptr_t Passengers = 0xC70;
        constexpr uintptr_t Boost = 0x320;
        constexpr uintptr_t Gravity = 0xBCC;
        constexpr uintptr_t EngineHealth = 0x908;
        constexpr uintptr_t BodyHealth = 0x90C;
        constexpr uintptr_t DirtLevel = 0x948;
        constexpr uintptr_t Godmode = 0x189;
    }

    // Weapon offsets
    namespace Weapon {
        constexpr uintptr_t CurrentWeapon = 0x20;
        constexpr uintptr_t AmmoInfo = 0x60;
        constexpr uintptr_t Ammo = 0x18;
        constexpr uintptr_t MaxAmmo = 0x28;
        constexpr uintptr_t WeaponHash = 0x10;
        constexpr uintptr_t Spread = 0x74;
        constexpr uintptr_t Recoil = 0x2F4;
        constexpr uintptr_t Range = 0x28C;
        constexpr uintptr_t ReloadMultiplier = 0x134;
    }

    // Bone IDs for skeleton ESP
    namespace Bones {
        constexpr int Head = 0x796E;
        constexpr int Neck = 0x9995;
        constexpr int Spine3 = 0x60F0;
        constexpr int Spine2 = 0x60F1;
        constexpr int Spine1 = 0x60F2;
        constexpr int Spine0 = 0xE0FD;
        constexpr int LeftShoulder = 0xB1C5;
        constexpr int LeftElbow = 0x58B7;
        constexpr int LeftHand = 0x49D9;
        constexpr int RightShoulder = 0x9D4D;
        constexpr int RightElbow = 0xBB0;
        constexpr int RightHand = 0xDEAD;
        constexpr int LeftHip = 0xE39F;
        constexpr int LeftKnee = 0xF9BB;
        constexpr int LeftFoot = 0x3779;
        constexpr int RightHip = 0xCA72;
        constexpr int RightKnee = 0x6E5C;
        constexpr int RightFoot = 0xCC4D;
    }
}

// Game structures
struct Vector3 {
    float x, y, z;

    Vector3() : x(0), y(0), z(0) {}
    Vector3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}

    float Distance(const Vector3& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        float dz = z - other.z;
        return sqrtf(dx * dx + dy * dy + dz * dz);
    }

    Vector3 operator-(const Vector3& other) const {
        return Vector3(x - other.x, y - other.y, z - other.z);
    }

    Vector3 operator+(const Vector3& other) const {
        return Vector3(x + other.x, y + other.y, z + other.z);
    }
};

struct Vector2 {
    float x, y;

    Vector2() : x(0), y(0) {}
    Vector2(float _x, float _y) : x(_x), y(_y) {}

    float Distance(const Vector2& other) const {
        float dx = x - other.x;
        float dy = y - other.y;
        return sqrtf(dx * dx + dy * dy);
    }
};

struct ViewMatrix {
    float matrix[4][4];
};

struct EntityInfo {
    uintptr_t address;
    Vector3 position;
    float health;
    float maxHealth;
    float armour;
    int type;
    bool isPlayer;
    float distance;
};

struct BoneMatrix {
    float data[3][4];
};

// Entity types
enum class EntityType : int {
    Ped = 4,
    Vehicle = 5,
    Object = 3,
    Pickup = 6
};

// Weapon hashes (common ones)
namespace WeaponHash {
    constexpr uint32_t Pistol = 0x1B06D571;
    constexpr uint32_t CombatPistol = 0x5EF9FEC4;
    constexpr uint32_t APPistol = 0x22D8FE39;
    constexpr uint32_t Micro_SMG = 0x13532244;
    constexpr uint32_t AssaultRifle = 0xBFEFFF6D;
    constexpr uint32_t CarbineRifle = 0x83BF0278;
    constexpr uint32_t AdvancedRifle = 0xAF113F99;
    constexpr uint32_t SniperRifle = 0x05FC3C11;
    constexpr uint32_t HeavySniper = 0x0C472FE2;
    constexpr uint32_t RPG = 0xB1CA77B1;
}
