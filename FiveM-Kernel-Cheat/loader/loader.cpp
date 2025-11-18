#include <Windows.h>
#include <iostream>
#include <string>

bool LoadDriver(const std::wstring& driverPath) {
    SC_HANDLE scManager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!scManager) {
        std::wcerr << L"[-] Failed to open service manager. Error: " << GetLastError() << std::endl;
        std::wcerr << L"[-] Make sure you're running as Administrator!" << std::endl;
        return false;
    }

    // Create service
    SC_HANDLE service = CreateServiceW(
        scManager,
        L"FiveMKernel",
        L"FiveM Kernel Driver",
        SERVICE_START | SERVICE_STOP | DELETE,
        SERVICE_KERNEL_DRIVER,
        SERVICE_DEMAND_START,
        SERVICE_ERROR_IGNORE,
        driverPath.c_str(),
        nullptr, nullptr, nullptr, nullptr, nullptr
    );

    if (!service) {
        DWORD error = GetLastError();
        if (error == ERROR_SERVICE_EXISTS) {
            std::wcout << L"[*] Service already exists, opening existing service..." << std::endl;
            service = OpenServiceW(scManager, L"FiveMKernel", SERVICE_START | SERVICE_STOP | DELETE);
            if (!service) {
                std::wcerr << L"[-] Failed to open existing service. Error: " << GetLastError() << std::endl;
                CloseServiceHandle(scManager);
                return false;
            }
        } else {
            std::wcerr << L"[-] Failed to create service. Error: " << error << std::endl;
            CloseServiceHandle(scManager);
            return false;
        }
    } else {
        std::wcout << L"[+] Service created successfully" << std::endl;
    }

    // Start service
    if (!StartServiceW(service, 0, nullptr)) {
        DWORD error = GetLastError();
        if (error == ERROR_SERVICE_ALREADY_RUNNING) {
            std::wcout << L"[*] Service is already running" << std::endl;
        } else {
            std::wcerr << L"[-] Failed to start service. Error: " << error << std::endl;
            DeleteService(service);
            CloseServiceHandle(service);
            CloseServiceHandle(scManager);
            return false;
        }
    } else {
        std::wcout << L"[+] Service started successfully" << std::endl;
    }

    CloseServiceHandle(service);
    CloseServiceHandle(scManager);

    std::wcout << L"[+] Driver loaded successfully!" << std::endl;
    return true;
}

bool UnloadDriver() {
    SC_HANDLE scManager = OpenSCManagerW(nullptr, nullptr, SC_MANAGER_CREATE_SERVICE);
    if (!scManager) {
        std::wcerr << L"[-] Failed to open service manager. Error: " << GetLastError() << std::endl;
        return false;
    }

    SC_HANDLE service = OpenServiceW(scManager, L"FiveMKernel", SERVICE_STOP | DELETE);
    if (!service) {
        std::wcerr << L"[-] Failed to open service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(scManager);
        return false;
    }

    // Stop service
    SERVICE_STATUS status;
    if (!ControlService(service, SERVICE_CONTROL_STOP, &status)) {
        DWORD error = GetLastError();
        if (error != ERROR_SERVICE_NOT_ACTIVE) {
            std::wcerr << L"[-] Failed to stop service. Error: " << error << std::endl;
        } else {
            std::wcout << L"[*] Service was not running" << std::endl;
        }
    } else {
        std::wcout << L"[+] Service stopped successfully" << std::endl;
    }

    // Delete service
    if (!DeleteService(service)) {
        std::wcerr << L"[-] Failed to delete service. Error: " << GetLastError() << std::endl;
        CloseServiceHandle(service);
        CloseServiceHandle(scManager);
        return false;
    }

    std::wcout << L"[+] Service deleted successfully" << std::endl;

    CloseServiceHandle(service);
    CloseServiceHandle(scManager);

    std::wcout << L"[+] Driver unloaded successfully!" << std::endl;
    return true;
}

void PrintUsage() {
    std::wcout << L"\nFiveM Kernel Driver Loader\n" << std::endl;
    std::wcout << L"Usage:" << std::endl;
    std::wcout << L"  loader.exe load <path_to_driver.sys>   - Load the kernel driver" << std::endl;
    std::wcout << L"  loader.exe unload                      - Unload the kernel driver" << std::endl;
    std::wcout << L"\nNote: Must be run as Administrator!\n" << std::endl;
}

bool EnableDebugPrivilege() {
    HANDLE token;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &token)) {
        return false;
    }

    LUID luid;
    if (!LookupPrivilegeValue(nullptr, SE_LOAD_DRIVER_NAME, &luid)) {
        CloseHandle(token);
        return false;
    }

    TOKEN_PRIVILEGES tp;
    tp.PrivilegeCount = 1;
    tp.Privileges[0].Luid = luid;
    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

    bool result = AdjustTokenPrivileges(token, FALSE, &tp, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr);
    CloseHandle(token);

    return result;
}

int wmain(int argc, wchar_t* argv[]) {
    SetConsoleTitleW(L"FiveM Kernel Driver Loader");

    std::wcout << L"╔══════════════════════════════════════════╗" << std::endl;
    std::wcout << L"║  FiveM Kernel Driver Loader v1.0         ║" << std::endl;
    std::wcout << L"╚══════════════════════════════════════════╝\n" << std::endl;

    if (argc < 2) {
        PrintUsage();
        return 1;
    }

    std::wstring command = argv[1];

    // Check for admin privileges
    BOOL isAdmin = FALSE;
    PSID adminGroup = nullptr;
    SID_IDENTIFIER_AUTHORITY ntAuthority = SECURITY_NT_AUTHORITY;

    if (AllocateAndInitializeSid(&ntAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID,
        DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0, &adminGroup)) {
        CheckTokenMembership(nullptr, adminGroup, &isAdmin);
        FreeSid(adminGroup);
    }

    if (!isAdmin) {
        std::wcerr << L"[-] ERROR: This program must be run as Administrator!" << std::endl;
        std::wcerr << L"[-] Right-click and select 'Run as Administrator'" << std::endl;
        return 1;
    }

    EnableDebugPrivilege();

    if (command == L"load") {
        if (argc < 3) {
            std::wcerr << L"[-] Please specify the driver path!" << std::endl;
            PrintUsage();
            return 1;
        }

        std::wstring driverPath = argv[2];

        // Check if file exists
        if (GetFileAttributesW(driverPath.c_str()) == INVALID_FILE_ATTRIBUTES) {
            std::wcerr << L"[-] Driver file not found: " << driverPath << std::endl;
            return 1;
        }

        // Convert to absolute path if needed
        wchar_t fullPath[MAX_PATH];
        GetFullPathNameW(driverPath.c_str(), MAX_PATH, fullPath, nullptr);
        driverPath = fullPath;

        std::wcout << L"[*] Loading driver from: " << driverPath << std::endl;

        if (LoadDriver(driverPath)) {
            std::wcout << L"\n[+] SUCCESS! Driver is now loaded and ready." << std::endl;
            std::wcout << L"[+] You can now run the client application." << std::endl;
            return 0;
        } else {
            std::wcerr << L"\n[-] FAILED to load driver!" << std::endl;
            std::wcerr << L"[-] Common issues:" << std::endl;
            std::wcerr << L"    - Test signing not enabled (run: bcdedit /set testsigning on)" << std::endl;
            std::wcerr << L"    - Driver not signed" << std::endl;
            std::wcerr << L"    - Incompatible Windows version" << std::endl;
            std::wcerr << L"    - Check Windows Event Viewer for details" << std::endl;
            return 1;
        }
    } else if (command == L"unload") {
        std::wcout << L"[*] Unloading driver..." << std::endl;

        if (UnloadDriver()) {
            std::wcout << L"\n[+] SUCCESS! Driver has been unloaded." << std::endl;
            return 0;
        } else {
            std::wcerr << L"\n[-] FAILED to unload driver!" << std::endl;
            return 1;
        }
    } else {
        std::wcerr << L"[-] Unknown command: " << command << std::endl;
        PrintUsage();
        return 1;
    }

    return 0;
}
