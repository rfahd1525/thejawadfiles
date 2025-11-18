#pragma once
#include "driver_interface.h"
#include "game_data.h"
#include "offset_scanner.h"
#include <vector>
#include <TlHelp32.h>

// Enable automatic offset scanning if offsets fail
#define AUTO_SCAN_ON_FAILURE 1

class Game {
private:
    DriverInterface* driver;
    DWORD processId;
    uintptr_t baseAddress;
    uintptr_t worldPtr;
    uintptr_t replayInterface;
    uintptr_t viewportPtr;
    bool usedAutoScan;

public:
    Game(DriverInterface* _driver) : driver(_driver), processId(0), baseAddress(0), usedAutoScan(false) {}

    bool Initialize(bool forceAutoScan = false) {
        // Find FiveM/GTA V process
        processId = GetProcessId(L"FiveM.exe");
        if (!processId) {
            processId = GetProcessId(L"GTA5.exe");
        }

        if (!processId) {
            std::cerr << "[-] Failed to find FiveM/GTA5 process" << std::endl;
            return false;
        }

        std::cout << "[+] Found process with PID: " << processId << std::endl;

        // Get base address
        baseAddress = driver->GetModuleBase(processId, L"GTA5.exe");
        if (!baseAddress) {
            std::cerr << "[-] Failed to get base address" << std::endl;
            return false;
        }

        std::cout << "[+] Base address: 0x" << std::hex << baseAddress << std::dec << std::endl;

        // Try automatic scanning if forced or if validation fails
        if (forceAutoScan || !ValidateOffsets()) {
            std::cout << "\n[!] Using automatic offset scanner..." << std::endl;

            OffsetScanner scanner(driver, processId, baseAddress);

            // Scan for all offsets
            if (scanner.ScanAll()) {
                std::cout << "[+] Automatic scan completed successfully!" << std::endl;

                // Update pointers with scanned offsets
                worldPtr = baseAddress + scanner.FindOffset("WorldPtr");
                replayInterface = baseAddress + scanner.FindOffset("ReplayInterface");
                viewportPtr = baseAddress + scanner.FindOffset("ViewPort");

                usedAutoScan = true;

                // Optional: Export to file for future use
                std::cout << "\n[*] Export scanned offsets to file? (y/n): ";
                char choice;
                std::cin >> choice;
                if (choice == 'y' || choice == 'Y') {
                    scanner.ExportToHeader("auto_offsets.h");
                    std::cout << "[+] Update game_data.h with these values for better performance" << std::endl;
                }
            } else {
                std::cerr << "[-] Automatic scan failed. Some features may not work." << std::endl;
                // Fall back to static offsets anyway
                worldPtr = baseAddress + Offsets::WorldPtr;
                replayInterface = baseAddress + Offsets::ReplayInterface;
                viewportPtr = baseAddress + Offsets::ViewPort;
            }
        } else {
            // Use static offsets from game_data.h
            std::cout << "[+] Using static offsets (validated)" << std::endl;
            worldPtr = baseAddress + Offsets::WorldPtr;
            replayInterface = baseAddress + Offsets::ReplayInterface;
            viewportPtr = baseAddress + Offsets::ViewPort;
        }

        // Protect our process from being terminated
        driver->ProtectProcess(GetCurrentProcessId(), true);

        return true;
    }

    DWORD GetProcessId(const wchar_t* processName) {
        HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot == INVALID_HANDLE_VALUE) {
            return 0;
        }

        PROCESSENTRY32W entry = {};
        entry.dwSize = sizeof(entry);

        if (Process32FirstW(snapshot, &entry)) {
            do {
                if (_wcsicmp(entry.szExeFile, processName) == 0) {
                    CloseHandle(snapshot);
                    return entry.th32ProcessID;
                }
            } while (Process32NextW(snapshot, &entry));
        }

        CloseHandle(snapshot);
        return 0;
    }

    bool ValidateOffsets() {
        // Quick validation to check if current offsets work
        std::cout << "[*] Validating static offsets..." << std::endl;

        // Test 1: Check if WorldPtr points to valid memory
        uintptr_t testWorld = driver->Read<uintptr_t>(processId, baseAddress + Offsets::WorldPtr);
        if (!testWorld || testWorld == 0xFFFFFFFFFFFFFFFF) {
            std::cout << "[!] WorldPtr validation failed" << std::endl;
            return false;
        }

        // Test 2: Check if we can read player info
        uintptr_t playerInfo = driver->Read<uintptr_t>(processId, testWorld + Offsets::Player::PlayerInfo);
        if (!playerInfo || playerInfo == 0xFFFFFFFFFFFFFFFF) {
            std::cout << "[!] PlayerInfo validation failed" << std::endl;
            return false;
        }

        // Test 3: Check if PedPool has reasonable count
        uintptr_t pedPool = driver->Read<uintptr_t>(processId, baseAddress + Offsets::PedPool);
        if (pedPool) {
            int pedCount = driver->Read<int>(processId, pedPool + 0x10);
            if (pedCount < 0 || pedCount > 10000) {
                std::cout << "[!] PedPool validation failed (unreasonable count: " << pedCount << ")" << std::endl;
                return false;
            }
        }

        std::cout << "[+] Static offsets validated successfully" << std::endl;
        return true;
    }

    uintptr_t GetLocalPlayer() {
        uintptr_t world = driver->Read<uintptr_t>(processId, worldPtr);
        if (!world) return 0;

        uintptr_t playerInfo = driver->Read<uintptr_t>(processId, world + Offsets::Player::PlayerInfo);
        if (!playerInfo) return 0;

        return driver->Read<uintptr_t>(processId, playerInfo + Offsets::Player::PlayerPed);
    }

    Vector3 GetEntityPosition(uintptr_t entity) {
        if (!entity) return Vector3();
        return driver->Read<Vector3>(processId, entity + Offsets::Entity::Coordinates);
    }

    float GetEntityHealth(uintptr_t entity) {
        if (!entity) return 0.0f;
        return driver->Read<float>(processId, entity + Offsets::Entity::Health);
    }

    float GetEntityMaxHealth(uintptr_t entity) {
        if (!entity) return 0.0f;
        return driver->Read<float>(processId, entity + Offsets::Entity::MaxHealth);
    }

    float GetEntityArmour(uintptr_t entity) {
        if (!entity) return 0.0f;
        return driver->Read<float>(processId, entity + Offsets::Entity::Armour);
    }

    int GetEntityType(uintptr_t entity) {
        if (!entity) return 0;
        return driver->Read<int>(processId, entity + Offsets::Entity::EntityType);
    }

    Vector3 GetBonePosition(uintptr_t ped, int boneId) {
        if (!ped) return Vector3();

        uintptr_t boneManager = driver->Read<uintptr_t>(processId, ped + Offsets::Ped::BoneManager);
        if (!boneManager) return Vector3();

        uintptr_t boneArray = driver->Read<uintptr_t>(processId, boneManager + 0x0);
        if (!boneArray) return Vector3();

        // Find bone index
        for (int i = 0; i < 256; i++) {
            int currentBoneId = driver->Read<int>(processId, boneArray + (i * 0x10));
            if (currentBoneId == boneId) {
                BoneMatrix matrix = driver->Read<BoneMatrix>(processId, boneArray + (i * 0x10) + 0x20);
                return Vector3(matrix.data[0][3], matrix.data[1][3], matrix.data[2][3]);
            }
        }

        return Vector3();
    }

    ViewMatrix GetViewMatrix() {
        return driver->Read<ViewMatrix>(processId, viewportPtr);
    }

    std::vector<EntityInfo> GetNearbyEntities(float maxDistance = 500.0f) {
        std::vector<EntityInfo> entities;

        uintptr_t localPlayer = GetLocalPlayer();
        if (!localPlayer) return entities;

        Vector3 localPos = GetEntityPosition(localPlayer);

        // Get ped pool
        uintptr_t pedPool = driver->Read<uintptr_t>(processId, baseAddress + Offsets::PedPool);
        if (pedPool) {
            int pedCount = driver->Read<int>(processId, pedPool + 0x10);
            uintptr_t pedList = driver->Read<uintptr_t>(processId, pedPool + 0x0);

            for (int i = 0; i < pedCount && i < 256; i++) {
                uintptr_t ped = driver->Read<uintptr_t>(processId, pedList + (i * 0x8));
                if (!ped || ped == localPlayer) continue;

                Vector3 pos = GetEntityPosition(ped);
                float distance = localPos.Distance(pos);

                if (distance > 0 && distance < maxDistance) {
                    EntityInfo info;
                    info.address = ped;
                    info.position = pos;
                    info.health = GetEntityHealth(ped);
                    info.maxHealth = GetEntityMaxHealth(ped);
                    info.armour = GetEntityArmour(ped);
                    info.type = static_cast<int>(EntityType::Ped);
                    info.isPlayer = true;
                    info.distance = distance;

                    entities.push_back(info);
                }
            }
        }

        // Get vehicle pool
        uintptr_t vehiclePool = driver->Read<uintptr_t>(processId, baseAddress + Offsets::VehiclePool);
        if (vehiclePool) {
            int vehicleCount = driver->Read<int>(processId, vehiclePool + 0x10);
            uintptr_t vehicleList = driver->Read<uintptr_t>(processId, vehiclePool + 0x0);

            for (int i = 0; i < vehicleCount && i < 256; i++) {
                uintptr_t vehicle = driver->Read<uintptr_t>(processId, vehicleList + (i * 0x8));
                if (!vehicle) continue;

                Vector3 pos = GetEntityPosition(vehicle);
                float distance = localPos.Distance(pos);

                if (distance > 0 && distance < maxDistance) {
                    EntityInfo info;
                    info.address = vehicle;
                    info.position = pos;
                    info.health = GetEntityHealth(vehicle);
                    info.maxHealth = GetEntityMaxHealth(vehicle);
                    info.armour = 0;
                    info.type = static_cast<int>(EntityType::Vehicle);
                    info.isPlayer = false;
                    info.distance = distance;

                    entities.push_back(info);
                }
            }
        }

        return entities;
    }

    void SetEntityHealth(uintptr_t entity, float health) {
        driver->Write<float>(processId, entity + Offsets::Entity::Health, health);
    }

    void SetEntityGodmode(uintptr_t entity, bool enabled) {
        driver->Write<BYTE>(processId, entity + Offsets::Player::Godmode, enabled ? 1 : 0);
    }

    void SetWantedLevel(int level) {
        uintptr_t localPlayer = GetLocalPlayer();
        if (!localPlayer) return;

        uintptr_t world = driver->Read<uintptr_t>(processId, worldPtr);
        if (!world) return;

        uintptr_t playerInfo = driver->Read<uintptr_t>(processId, world + Offsets::Player::PlayerInfo);
        if (!playerInfo) return;

        driver->Write<int>(processId, playerInfo + Offsets::Player::WantedLevel, level);
    }

    void SetInfiniteAmmo(bool enabled) {
        uintptr_t localPlayer = GetLocalPlayer();
        if (!localPlayer) return;

        uintptr_t weaponManager = driver->Read<uintptr_t>(processId, localPlayer + Offsets::Ped::WeaponManager);
        if (!weaponManager) return;

        uintptr_t currentWeapon = driver->Read<uintptr_t>(processId, weaponManager + Offsets::Weapon::CurrentWeapon);
        if (!currentWeapon) return;

        if (enabled) {
            driver->Write<int>(processId, currentWeapon + Offsets::Weapon::Ammo, 9999);
        }
    }

    void SetNoRecoil(bool enabled) {
        uintptr_t localPlayer = GetLocalPlayer();
        if (!localPlayer) return;

        uintptr_t weaponManager = driver->Read<uintptr_t>(processId, localPlayer + Offsets::Ped::WeaponManager);
        if (!weaponManager) return;

        uintptr_t currentWeapon = driver->Read<uintptr_t>(processId, weaponManager + Offsets::Weapon::CurrentWeapon);
        if (!currentWeapon) return;

        driver->Write<float>(processId, currentWeapon + Offsets::Weapon::Recoil, enabled ? 0.0f : 1.0f);
        driver->Write<float>(processId, currentWeapon + Offsets::Weapon::Spread, enabled ? 0.0f : 1.0f);
    }

    void SetSuperSpeed(bool enabled) {
        uintptr_t localPlayer = GetLocalPlayer();
        if (!localPlayer) return;

        Vector3 velocity = driver->Read<Vector3>(processId, localPlayer + Offsets::Entity::Velocity);
        if (enabled) {
            velocity.x *= 2.0f;
            velocity.y *= 2.0f;
            driver->Write<Vector3>(processId, localPlayer + Offsets::Entity::Velocity, velocity);
        }
    }

    void TeleportToWaypoint() {
        uintptr_t waypointPtr = baseAddress + Offsets::Waypoint;
        Vector3 waypoint = driver->Read<Vector3>(processId, waypointPtr);

        uintptr_t localPlayer = GetLocalPlayer();
        if (!localPlayer) return;

        waypoint.z = 1000.0f; // Set high z to fall down to ground
        driver->Write<Vector3>(processId, localPlayer + Offsets::Entity::Coordinates, waypoint);
    }

    DWORD GetPID() { return processId; }
    uintptr_t GetBase() { return baseAddress; }
};
