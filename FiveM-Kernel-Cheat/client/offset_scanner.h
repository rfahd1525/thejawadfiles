#pragma once
#include "driver_interface.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>

// Automatic offset scanner using pattern matching
// Patterns are based on assembly signatures near the data we need

class OffsetScanner {
private:
    DriverInterface* driver;
    DWORD processId;
    uintptr_t baseAddress;

    struct Pattern {
        const char* name;
        const char* pattern;
        const char* mask;
        int offset;  // Offset from pattern match to actual address
        bool isPointer;  // If true, dereference the result
    };

    // GTA V Assembly Patterns (updated for latest builds)
    // These patterns are more stable across updates than direct offsets
    std::vector<Pattern> patterns = {
        // World pointer pattern
        // Signature: 48 8B 05 ? ? ? ? 45 33 C0 48 8B 48 08 48 85 C9
        {
            "WorldPtr",
            "\x48\x8B\x05\x00\x00\x00\x00\x45\x00\xC0\x48\x8B\x48",
            "xxx????xx?xxxx",
            3,
            true
        },

        // Replay Interface pattern
        // Signature: 48 8B 0D ? ? ? ? 48 85 C9 74 ? 48 8B 01 FF 50
        {
            "ReplayInterface",
            "\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x74\x00\x48\x8B\x01\xFF\x50",
            "xxx????xxxx?xxxxx",
            3,
            true
        },

        // ViewPort pattern
        // Signature: 48 8B 15 ? ? ? ? 48 8D 2D ? ? ? ? 48 8B CD
        {
            "ViewPort",
            "\x48\x8B\x15\x00\x00\x00\x00\x48\x8D\x2D\x00\x00\x00\x00\x48\x8B\xCD",
            "xxx????xxx????xxx",
            3,
            true
        },

        // Ped Pool pattern
        // Signature: 48 8B 05 ? ? ? ? 41 0F BF C8 0F BF 40 10
        {
            "PedPool",
            "\x48\x8B\x05\x00\x00\x00\x00\x41\x0F\xBF\xC8\x0F\xBF\x40\x10",
            "xxx????xxxxxxxx",
            3,
            true
        },

        // Vehicle Pool pattern
        // Signature: 48 8B 05 ? ? ? ? F3 0F 59 F6 48 8B 08
        {
            "VehiclePool",
            "\x48\x8B\x05\x00\x00\x00\x00\xF3\x0F\x59\xF6\x48\x8B\x08",
            "xxx????xxxxxxx",
            3,
            true
        },

        // Object Pool pattern
        {
            "ObjectPool",
            "\x48\x8B\x05\x00\x00\x00\x00\x8B\x78\x10\x85\xFF",
            "xxx????xxxxx",
            3,
            true
        },

        // Waypoint pattern
        {
            "Waypoint",
            "\x48\x8B\x1D\x00\x00\x00\x00\x48\x85\xDB\x74\x00\xF3\x0F\x10\x43",
            "xxx????xxxx?xxxx",
            3,
            true
        },

        // Local Scripts pattern
        {
            "LocalScripts",
            "\x48\x8B\x0D\x00\x00\x00\x00\x4C\x8D\x0D\x00\x00\x00\x00\x4D\x8B\x01",
            "xxx????xxx????xxx",
            3,
            true
        }
    };

public:
    OffsetScanner(DriverInterface* _driver, DWORD _processId, uintptr_t _baseAddress)
        : driver(_driver), processId(_processId), baseAddress(_baseAddress) {}

    // Scan for a specific pattern
    uintptr_t ScanPattern(const char* pattern, const char* mask, SIZE_T scanSize = 0x10000000) {
        std::cout << "[*] Scanning for pattern (mask: " << mask << ")..." << std::endl;

        // Scan in chunks to avoid memory issues
        SIZE_T chunkSize = 0x100000; // 1MB chunks
        std::vector<BYTE> buffer(chunkSize);

        for (SIZE_T offset = 0; offset < scanSize; offset += chunkSize) {
            if (!driver->ReadBuffer(processId, baseAddress + offset, buffer.data(), chunkSize)) {
                continue;
            }

            SIZE_T patternLength = strlen(mask);

            for (SIZE_T i = 0; i < chunkSize - patternLength; i++) {
                bool found = true;

                for (SIZE_T j = 0; j < patternLength; j++) {
                    if (mask[j] != '?' && buffer[i + j] != (BYTE)pattern[j]) {
                        found = false;
                        break;
                    }
                }

                if (found) {
                    uintptr_t address = baseAddress + offset + i;
                    std::cout << "[+] Pattern found at: 0x" << std::hex << address << std::dec << std::endl;
                    return address;
                }
            }
        }

        std::cout << "[-] Pattern not found" << std::endl;
        return 0;
    }

    // Resolve RIP-relative address (common in x64)
    uintptr_t ResolveRIP(uintptr_t instructionAddress, int offset = 3, int instructionSize = 7) {
        // Read the 4-byte relative offset
        int32_t relativeOffset = driver->Read<int32_t>(processId, instructionAddress + offset);

        // Calculate absolute address: instruction_address + instruction_size + relative_offset
        uintptr_t absoluteAddress = instructionAddress + instructionSize + relativeOffset;

        std::cout << "[*] RIP-relative: 0x" << std::hex << instructionAddress
                  << " + " << instructionSize << " + " << relativeOffset
                  << " = 0x" << absoluteAddress << std::dec << std::endl;

        return absoluteAddress;
    }

    // Scan and resolve all known patterns
    bool ScanAll() {
        std::cout << "\n========================================" << std::endl;
        std::cout << "  Automatic Offset Scanner" << std::endl;
        std::cout << "========================================\n" << std::endl;

        bool allFound = true;

        for (const auto& pattern : patterns) {
            std::cout << "[*] Searching for: " << pattern.name << std::endl;

            uintptr_t patternAddress = ScanPattern(pattern.pattern, pattern.mask);

            if (patternAddress) {
                uintptr_t resolvedAddress = ResolveRIP(patternAddress, pattern.offset);

                if (pattern.isPointer) {
                    uintptr_t finalAddress = driver->Read<uintptr_t>(processId, resolvedAddress);
                    std::cout << "[+] " << pattern.name << " final address: 0x"
                              << std::hex << finalAddress << std::dec << std::endl;

                    // Calculate offset from base
                    uintptr_t offset = resolvedAddress - baseAddress;
                    std::cout << "[+] " << pattern.name << " offset: 0x"
                              << std::hex << offset << std::dec << std::endl;
                } else {
                    uintptr_t offset = resolvedAddress - baseAddress;
                    std::cout << "[+] " << pattern.name << " offset: 0x"
                              << std::hex << offset << std::dec << std::endl;
                }

                std::cout << std::endl;
            } else {
                std::cout << "[-] Failed to find: " << pattern.name << "\n" << std::endl;
                allFound = false;
            }
        }

        return allFound;
    }

    // Scan for a specific offset by name
    uintptr_t FindOffset(const std::string& name) {
        for (const auto& pattern : patterns) {
            if (pattern.name == name) {
                uintptr_t patternAddress = ScanPattern(pattern.pattern, pattern.mask);
                if (patternAddress) {
                    uintptr_t resolvedAddress = ResolveRIP(patternAddress, pattern.offset);
                    return resolvedAddress - baseAddress;
                }
            }
        }
        return 0;
    }

    // Export offsets to header file
    void ExportToHeader(const std::string& filename) {
        std::ofstream file(filename);

        file << "#pragma once\n\n";
        file << "// Auto-generated offsets\n";
        file << "// Game Version: " << GetGameVersion() << "\n\n";
        file << "namespace AutoOffsets {\n";

        for (const auto& pattern : patterns) {
            uintptr_t offset = FindOffset(pattern.name);
            if (offset) {
                file << "    constexpr uintptr_t " << pattern.name
                     << " = 0x" << std::hex << offset << std::dec << ";\n";
            }
        }

        file << "}\n";
        file.close();

        std::cout << "[+] Offsets exported to: " << filename << std::endl;
    }

    // Get game version
    std::string GetGameVersion() {
        // Try to read version from game memory
        // This is a placeholder - actual implementation would read from game's version info
        return "Unknown (Auto-scanned)";
    }

    // Verify offset by testing if data at that location makes sense
    bool VerifyOffset(const std::string& name, uintptr_t offset) {
        if (name == "WorldPtr") {
            // World pointer should point to valid memory
            uintptr_t worldAddr = driver->Read<uintptr_t>(processId, baseAddress + offset);
            return worldAddr != 0 && worldAddr < 0x7FFFFFFFFFFF;
        }
        else if (name == "PedPool" || name == "VehiclePool") {
            // Pool should have reasonable count
            uintptr_t poolAddr = driver->Read<uintptr_t>(processId, baseAddress + offset);
            if (poolAddr) {
                int count = driver->Read<int>(processId, poolAddr + 0x10);
                return count >= 0 && count < 10000; // Reasonable entity count
            }
        }

        return true; // Default to valid
    }
};
