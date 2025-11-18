#include <iostream>
#include <thread>
#include <chrono>
#include "driver_interface.h"
#include "game.h"
#include "overlay.h"

void PrintBanner() {
    std::cout << R"(
    ███████╗██╗██╗   ██╗███████╗███╗   ███╗    ██╗  ██╗███████╗██████╗ ███╗   ██╗███████╗██╗
    ██╔════╝██║██║   ██║██╔════╝████╗ ████║    ██║ ██╔╝██╔════╝██╔══██╗████╗  ██║██╔════╝██║
    █████╗  ██║██║   ██║█████╗  ██╔████╔██║    █████╔╝ █████╗  ██████╔╝██╔██╗ ██║█████╗  ██║
    ██╔══╝  ██║╚██╗ ██╔╝██╔══╝  ██║╚██╔╝██║    ██╔═██╗ ██╔══╝  ██╔══██╗██║╚██╗██║██╔══╝  ██║
    ██║     ██║ ╚████╔╝ ███████╗██║ ╚═╝ ██║    ██║  ██╗███████╗██║  ██║██║ ╚████║███████╗███████╗
    ╚═╝     ╚═╝  ╚═══╝  ╚══════╝╚═╝     ╚═╝    ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝╚══════╝╚══════╝

    FiveM Kernel-Level Cheat | Educational Build
    Press INSERT to toggle menu

    )" << std::endl;
}

int main() {
    SetConsoleTitleA("FiveM Kernel Cheat");
    PrintBanner();

    std::cout << "[*] Initializing driver interface..." << std::endl;
    DriverInterface driver;

    if (!driver.Initialize()) {
        std::cerr << "[-] Failed to initialize driver interface" << std::endl;
        std::cerr << "[-] Make sure the kernel driver is loaded!" << std::endl;
        std::cin.get();
        return 1;
    }

    std::cout << "[*] Initializing game interface..." << std::endl;
    Game game(&driver);

    if (!game.Initialize()) {
        std::cerr << "[-] Failed to initialize game interface" << std::endl;
        std::cerr << "[-] Make sure FiveM/GTA5 is running!" << std::endl;
        std::cin.get();
        return 1;
    }

    std::cout << "[*] Creating overlay..." << std::endl;
    Overlay overlay(&game);

    if (!overlay.Initialize()) {
        std::cerr << "[-] Failed to initialize overlay" << std::endl;
        std::cin.get();
        return 1;
    }

    std::cout << "[+] All systems initialized successfully!" << std::endl;
    std::cout << "[+] Overlay is running. Press INSERT to toggle menu." << std::endl;
    std::cout << "\n[*] Features:" << std::endl;
    std::cout << "    - ESP (Boxes, Skeleton, Distance, Health)" << std::endl;
    std::cout << "    - Aimbot (FOV-based with smoothing)" << std::endl;
    std::cout << "    - Godmode" << std::endl;
    std::cout << "    - Infinite Ammo" << std::endl;
    std::cout << "    - No Recoil/Spread" << std::endl;
    std::cout << "    - Super Speed" << std::endl;
    std::cout << "    - Teleport to Waypoint" << std::endl;
    std::cout << "    - Clear Wanted Level" << std::endl;
    std::cout << "\n[*] Running... (Close this window to exit)\n" << std::endl;

    // Main loop
    while (overlay.ShouldRun()) {
        overlay.Render();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    std::cout << "[*] Shutting down..." << std::endl;
    overlay.Shutdown();

    return 0;
}
