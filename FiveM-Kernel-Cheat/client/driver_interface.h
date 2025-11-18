#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include "../shared/shared.h"

class DriverInterface {
private:
    HANDLE driverHandle;

public:
    DriverInterface() : driverHandle(INVALID_HANDLE_VALUE) {}

    ~DriverInterface() {
        if (driverHandle != INVALID_HANDLE_VALUE) {
            CloseHandle(driverHandle);
        }
    }

    bool Initialize() {
        driverHandle = CreateFileW(
            L"\\\\.\\FiveMKernel",
            GENERIC_READ | GENERIC_WRITE,
            0,
            nullptr,
            OPEN_EXISTING,
            FILE_ATTRIBUTE_NORMAL,
            nullptr
        );

        if (driverHandle == INVALID_HANDLE_VALUE) {
            std::cerr << "Failed to open driver handle. Error: " << GetLastError() << std::endl;
            return false;
        }

        std::cout << "[+] Connected to kernel driver" << std::endl;
        return true;
    }

    template<typename T>
    T Read(DWORD processId, uintptr_t address) {
        T buffer{};
        MEMORY_OPERATION memOp = {};
        memOp.ProcessId = processId;
        memOp.Address = reinterpret_cast<PVOID>(address);
        memOp.Buffer = &buffer;
        memOp.Size = sizeof(T);

        DWORD bytesReturned = 0;
        if (!DeviceIoControl(
            driverHandle,
            IOCTL_READ_MEMORY,
            &memOp,
            sizeof(memOp),
            &memOp,
            sizeof(memOp),
            &bytesReturned,
            nullptr
        )) {
            return T{};
        }

        return buffer;
    }

    bool ReadBuffer(DWORD processId, uintptr_t address, void* buffer, SIZE_T size) {
        MEMORY_OPERATION memOp = {};
        memOp.ProcessId = processId;
        memOp.Address = reinterpret_cast<PVOID>(address);
        memOp.Buffer = buffer;
        memOp.Size = size;

        DWORD bytesReturned = 0;
        return DeviceIoControl(
            driverHandle,
            IOCTL_READ_MEMORY,
            &memOp,
            sizeof(memOp),
            &memOp,
            sizeof(memOp),
            &bytesReturned,
            nullptr
        );
    }

    template<typename T>
    bool Write(DWORD processId, uintptr_t address, T value) {
        MEMORY_OPERATION memOp = {};
        memOp.ProcessId = processId;
        memOp.Address = reinterpret_cast<PVOID>(address);
        memOp.Buffer = &value;
        memOp.Size = sizeof(T);

        DWORD bytesReturned = 0;
        return DeviceIoControl(
            driverHandle,
            IOCTL_WRITE_MEMORY,
            &memOp,
            sizeof(memOp),
            &memOp,
            sizeof(memOp),
            &bytesReturned,
            nullptr
        );
    }

    bool WriteBuffer(DWORD processId, uintptr_t address, void* buffer, SIZE_T size) {
        MEMORY_OPERATION memOp = {};
        memOp.ProcessId = processId;
        memOp.Address = reinterpret_cast<PVOID>(address);
        memOp.Buffer = buffer;
        memOp.Size = size;

        DWORD bytesReturned = 0;
        return DeviceIoControl(
            driverHandle,
            IOCTL_WRITE_MEMORY,
            &memOp,
            sizeof(memOp),
            &memOp,
            sizeof(memOp),
            &bytesReturned,
            nullptr
        );
    }

    uintptr_t GetModuleBase(DWORD processId, const wchar_t* moduleName) {
        MODULE_OPERATION modOp = {};
        modOp.ProcessId = processId;
        wcscpy_s(modOp.ModuleName, moduleName);

        DWORD bytesReturned = 0;
        if (!DeviceIoControl(
            driverHandle,
            IOCTL_GET_MODULE_BASE,
            &modOp,
            sizeof(modOp),
            &modOp,
            sizeof(modOp),
            &bytesReturned,
            nullptr
        )) {
            return 0;
        }

        return reinterpret_cast<uintptr_t>(modOp.BaseAddress);
    }

    bool ProtectProcess(DWORD processId, bool protect) {
        PROTECT_OPERATION protOp = {};
        protOp.ProcessId = processId;
        protOp.Protect = protect;

        DWORD bytesReturned = 0;
        return DeviceIoControl(
            driverHandle,
            IOCTL_PROTECT_PROCESS,
            &protOp,
            sizeof(protOp),
            &protOp,
            sizeof(protOp),
            &bytesReturned,
            nullptr
        );
    }

    // Pattern scanning
    uintptr_t PatternScan(DWORD processId, uintptr_t baseAddress, SIZE_T size, const char* pattern, const char* mask) {
        std::vector<BYTE> buffer(size);

        if (!ReadBuffer(processId, baseAddress, buffer.data(), size)) {
            return 0;
        }

        size_t patternLength = strlen(mask);

        for (size_t i = 0; i < size - patternLength; i++) {
            bool found = true;
            for (size_t j = 0; j < patternLength; j++) {
                if (mask[j] != '?' && buffer[i + j] != pattern[j]) {
                    found = false;
                    break;
                }
            }

            if (found) {
                return baseAddress + i;
            }
        }

        return 0;
    }
};
