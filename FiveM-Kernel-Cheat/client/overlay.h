#pragma once
#include <d3d11.h>
#include <dxgi.h>
#include <Windows.h>
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "game.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

class Overlay {
private:
    HWND hwnd;
    WNDCLASSEXW wc;
    ID3D11Device* device;
    ID3D11DeviceContext* deviceContext;
    IDXGISwapChain* swapChain;
    ID3D11RenderTargetView* renderTargetView;

    int screenWidth;
    int screenHeight;

    // Feature toggles
    bool showMenu;
    bool espEnabled;
    bool espBoxes;
    bool espSkeleton;
    bool espDistance;
    bool espHealth;
    bool aimbotEnabled;
    bool godmodeEnabled;
    bool infiniteAmmoEnabled;
    bool noRecoilEnabled;
    bool superSpeedEnabled;

    float aimbotFOV;
    float aimbotSmooth;
    int aimbotKey;

    Game* game;

public:
    Overlay(Game* _game) :
        hwnd(nullptr),
        device(nullptr),
        deviceContext(nullptr),
        swapChain(nullptr),
        renderTargetView(nullptr),
        showMenu(true),
        espEnabled(true),
        espBoxes(true),
        espSkeleton(true),
        espDistance(true),
        espHealth(true),
        aimbotEnabled(false),
        godmodeEnabled(false),
        infiniteAmmoEnabled(false),
        noRecoilEnabled(false),
        superSpeedEnabled(false),
        aimbotFOV(120.0f),
        aimbotSmooth(5.0f),
        aimbotKey(VK_RBUTTON),
        game(_game)
    {
        screenWidth = GetSystemMetrics(SM_CXSCREEN);
        screenHeight = GetSystemMetrics(SM_CYSCREEN);
    }

    bool Initialize() {
        // Create window class
        wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr, nullptr, nullptr, L"FiveM Overlay", nullptr };
        RegisterClassExW(&wc);

        // Create window
        hwnd = CreateWindowExW(
            WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED,
            wc.lpszClassName,
            L"FiveM Overlay",
            WS_POPUP,
            0, 0, screenWidth, screenHeight,
            nullptr, nullptr, wc.hInstance, nullptr
        );

        if (!hwnd) {
            return false;
        }

        // Set layered window attributes
        SetLayeredWindowAttributes(hwnd, RGB(0, 0, 0), 255, LWA_ALPHA);

        // Extend frame into client area (for transparency)
        MARGINS margins = { -1, -1, -1, -1 };
        DwmExtendFrameIntoClientArea(hwnd, &margins);

        // Create DirectX device and swap chain
        DXGI_SWAP_CHAIN_DESC sd = {};
        sd.BufferCount = 2;
        sd.BufferDesc.Width = screenWidth;
        sd.BufferDesc.Height = screenHeight;
        sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        sd.BufferDesc.RefreshRate.Numerator = 60;
        sd.BufferDesc.RefreshRate.Denominator = 1;
        sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        sd.OutputWindow = hwnd;
        sd.SampleDesc.Count = 1;
        sd.SampleDesc.Quality = 0;
        sd.Windowed = TRUE;
        sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
        sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        D3D_FEATURE_LEVEL featureLevel;
        const D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

        HRESULT hr = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_HARDWARE,
            nullptr,
            0,
            featureLevels,
            2,
            D3D11_SDK_VERSION,
            &sd,
            &swapChain,
            &device,
            &featureLevel,
            &deviceContext
        );

        if (FAILED(hr)) {
            return false;
        }

        CreateRenderTarget();

        // Setup ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.IniFilename = nullptr;

        ImGui::StyleColorsDark();
        CustomStyle();

        ImGui_ImplWin32_Init(hwnd);
        ImGui_ImplDX11_Init(device, deviceContext);

        ShowWindow(hwnd, SW_SHOWDEFAULT);
        UpdateWindow(hwnd);

        std::cout << "[+] Overlay initialized" << std::endl;

        return true;
    }

    void CreateRenderTarget() {
        ID3D11Texture2D* backBuffer;
        swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer));
        device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
        backBuffer->Release();
    }

    void CleanupRenderTarget() {
        if (renderTargetView) {
            renderTargetView->Release();
            renderTargetView = nullptr;
        }
    }

    void CustomStyle() {
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowRounding = 5.0f;
        style.FrameRounding = 3.0f;
        style.ScrollbarRounding = 3.0f;
        style.GrabRounding = 3.0f;
        style.WindowBorderSize = 1.0f;
        style.FrameBorderSize = 1.0f;

        ImVec4* colors = style.Colors;
        colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.95f);
        colors[ImGuiCol_Border] = ImVec4(0.40f, 0.40f, 0.40f, 0.50f);
        colors[ImGuiCol_FrameBg] = ImVec4(0.20f, 0.20f, 0.20f, 0.54f);
        colors[ImGuiCol_FrameBgHovered] = ImVec4(0.30f, 0.30f, 0.30f, 0.40f);
        colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.35f, 0.67f);
        colors[ImGuiCol_TitleBg] = ImVec4(0.04f, 0.04f, 0.04f, 1.00f);
        colors[ImGuiCol_TitleBgActive] = ImVec4(0.15f, 0.15f, 0.15f, 1.00f);
        colors[ImGuiCol_CheckMark] = ImVec4(0.90f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.30f, 0.30f, 1.00f);
        colors[ImGuiCol_SliderGrabActive] = ImVec4(0.90f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_Button] = ImVec4(0.80f, 0.30f, 0.30f, 0.40f);
        colors[ImGuiCol_ButtonHovered] = ImVec4(0.90f, 0.40f, 0.40f, 1.00f);
        colors[ImGuiCol_ButtonActive] = ImVec4(0.95f, 0.45f, 0.45f, 1.00f);
        colors[ImGuiCol_Header] = ImVec4(0.80f, 0.30f, 0.30f, 0.31f);
        colors[ImGuiCol_HeaderHovered] = ImVec4(0.90f, 0.40f, 0.40f, 0.80f);
        colors[ImGuiCol_HeaderActive] = ImVec4(0.95f, 0.45f, 0.45f, 1.00f);
    }

    bool WorldToScreen(const Vector3& pos, Vector2& screen, const ViewMatrix& viewMatrix) {
        Vector4 clipCoords;
        clipCoords.x = pos.x * viewMatrix.matrix[0][0] + pos.y * viewMatrix.matrix[0][1] + pos.z * viewMatrix.matrix[0][2] + viewMatrix.matrix[0][3];
        clipCoords.y = pos.x * viewMatrix.matrix[1][0] + pos.y * viewMatrix.matrix[1][1] + pos.z * viewMatrix.matrix[1][2] + viewMatrix.matrix[1][3];
        clipCoords.z = pos.x * viewMatrix.matrix[2][0] + pos.y * viewMatrix.matrix[2][1] + pos.z * viewMatrix.matrix[2][2] + viewMatrix.matrix[2][3];
        clipCoords.w = pos.x * viewMatrix.matrix[3][0] + pos.y * viewMatrix.matrix[3][1] + pos.z * viewMatrix.matrix[3][2] + viewMatrix.matrix[3][3];

        if (clipCoords.w < 0.1f) {
            return false;
        }

        Vector3 ndc;
        ndc.x = clipCoords.x / clipCoords.w;
        ndc.y = clipCoords.y / clipCoords.w;
        ndc.z = clipCoords.z / clipCoords.w;

        screen.x = (screenWidth / 2.0f * ndc.x) + (ndc.x + screenWidth / 2.0f);
        screen.y = -(screenHeight / 2.0f * ndc.y) + (ndc.y + screenHeight / 2.0f);

        return true;
    }

    void DrawESP(ImDrawList* drawList) {
        if (!espEnabled) return;

        ViewMatrix viewMatrix = game->GetViewMatrix();
        std::vector<EntityInfo> entities = game->GetNearbyEntities(500.0f);

        for (const auto& entity : entities) {
            Vector2 screenPos;
            if (!WorldToScreen(entity.position, screenPos, viewMatrix)) continue;

            ImU32 color = entity.health > 0 ? IM_COL32(255, 0, 0, 255) : IM_COL32(100, 100, 100, 255);

            // Draw boxes
            if (espBoxes) {
                Vector3 headPos = entity.position;
                headPos.z += 1.0f;
                Vector2 headScreen;

                if (WorldToScreen(headPos, headScreen, viewMatrix)) {
                    float height = screenPos.y - headScreen.y;
                    float width = height / 2.0f;

                    drawList->AddRect(
                        ImVec2(screenPos.x - width / 2, headScreen.y),
                        ImVec2(screenPos.x + width / 2, screenPos.y),
                        color,
                        0.0f,
                        0,
                        2.0f
                    );

                    // Draw health bar
                    if (espHealth && entity.maxHealth > 0) {
                        float healthPercent = entity.health / entity.maxHealth;
                        float barHeight = height * healthPercent;

                        ImU32 healthColor = IM_COL32(
                            static_cast<int>(255 * (1.0f - healthPercent)),
                            static_cast<int>(255 * healthPercent),
                            0,
                            255
                        );

                        drawList->AddRectFilled(
                            ImVec2(screenPos.x - width / 2 - 6, screenPos.y - barHeight),
                            ImVec2(screenPos.x - width / 2 - 2, screenPos.y),
                            healthColor
                        );

                        drawList->AddRect(
                            ImVec2(screenPos.x - width / 2 - 6, headScreen.y),
                            ImVec2(screenPos.x - width / 2 - 2, screenPos.y),
                            IM_COL32(0, 0, 0, 255)
                        );
                    }
                }
            }

            // Draw skeleton
            if (espSkeleton && entity.type == static_cast<int>(EntityType::Ped)) {
                DrawSkeleton(drawList, entity.address, viewMatrix, color);
            }

            // Draw distance
            if (espDistance) {
                char distanceText[32];
                sprintf_s(distanceText, "%.0fm", entity.distance);

                drawList->AddText(
                    ImVec2(screenPos.x + 5, screenPos.y - 20),
                    IM_COL32(255, 255, 255, 255),
                    distanceText
                );
            }
        }
    }

    void DrawSkeleton(ImDrawList* drawList, uintptr_t ped, const ViewMatrix& viewMatrix, ImU32 color) {
        auto drawBone = [&](int bone1, int bone2) {
            Vector3 pos1 = game->GetBonePosition(ped, bone1);
            Vector3 pos2 = game->GetBonePosition(ped, bone2);

            Vector2 screen1, screen2;
            if (WorldToScreen(pos1, screen1, viewMatrix) && WorldToScreen(pos2, screen2, viewMatrix)) {
                drawList->AddLine(ImVec2(screen1.x, screen1.y), ImVec2(screen2.x, screen2.y), color, 2.0f);
            }
        };

        // Head to neck
        drawBone(Offsets::Bones::Head, Offsets::Bones::Neck);

        // Spine
        drawBone(Offsets::Bones::Neck, Offsets::Bones::Spine3);
        drawBone(Offsets::Bones::Spine3, Offsets::Bones::Spine2);
        drawBone(Offsets::Bones::Spine2, Offsets::Bones::Spine1);
        drawBone(Offsets::Bones::Spine1, Offsets::Bones::Spine0);

        // Left arm
        drawBone(Offsets::Bones::Neck, Offsets::Bones::LeftShoulder);
        drawBone(Offsets::Bones::LeftShoulder, Offsets::Bones::LeftElbow);
        drawBone(Offsets::Bones::LeftElbow, Offsets::Bones::LeftHand);

        // Right arm
        drawBone(Offsets::Bones::Neck, Offsets::Bones::RightShoulder);
        drawBone(Offsets::Bones::RightShoulder, Offsets::Bones::RightElbow);
        drawBone(Offsets::Bones::RightElbow, Offsets::Bones::RightHand);

        // Left leg
        drawBone(Offsets::Bones::Spine0, Offsets::Bones::LeftHip);
        drawBone(Offsets::Bones::LeftHip, Offsets::Bones::LeftKnee);
        drawBone(Offsets::Bones::LeftKnee, Offsets::Bones::LeftFoot);

        // Right leg
        drawBone(Offsets::Bones::Spine0, Offsets::Bones::RightHip);
        drawBone(Offsets::Bones::RightHip, Offsets::Bones::RightKnee);
        drawBone(Offsets::Bones::RightKnee, Offsets::Bones::RightFoot);
    }

    void ProcessAimbot() {
        if (!aimbotEnabled) return;
        if (!GetAsyncKeyState(aimbotKey)) return;

        uintptr_t localPlayer = game->GetLocalPlayer();
        if (!localPlayer) return;

        Vector3 localPos = game->GetEntityPosition(localPlayer);
        ViewMatrix viewMatrix = game->GetViewMatrix();

        float closestDistance = FLT_MAX;
        Vector3 targetPos;
        bool foundTarget = false;

        std::vector<EntityInfo> entities = game->GetNearbyEntities(300.0f);

        for (const auto& entity : entities) {
            if (entity.health <= 0) continue;
            if (entity.type != static_cast<int>(EntityType::Ped)) continue;

            Vector3 headPos = game->GetBonePosition(entity.address, Offsets::Bones::Head);
            Vector2 screenPos;

            if (!WorldToScreen(headPos, screenPos, viewMatrix)) continue;

            float centerX = screenWidth / 2.0f;
            float centerY = screenHeight / 2.0f;

            float distance = Vector2(screenPos.x - centerX, screenPos.y - centerY).Distance(Vector2(0, 0));
            float fovPixels = (screenWidth / 90.0f) * aimbotFOV;

            if (distance < fovPixels && distance < closestDistance) {
                closestDistance = distance;
                targetPos = headPos;
                foundTarget = true;
            }
        }

        if (foundTarget) {
            AimAtPosition(targetPos);
        }
    }

    void AimAtPosition(const Vector3& targetPos) {
        POINT cursorPos;
        GetCursorPos(&cursorPos);

        Vector2 screenTarget;
        ViewMatrix viewMatrix = game->GetViewMatrix();

        if (!WorldToScreen(targetPos, screenTarget, viewMatrix)) return;

        float deltaX = (screenTarget.x - cursorPos.x) / aimbotSmooth;
        float deltaY = (screenTarget.y - cursorPos.y) / aimbotSmooth;

        mouse_event(MOUSEEVENTF_MOVE, static_cast<DWORD>(deltaX), static_cast<DWORD>(deltaY), 0, 0);
    }

    void DrawMenu() {
        if (!showMenu) return;

        ImGui::SetNextWindowSize(ImVec2(500, 600), ImGuiCond_FirstUseEver);
        ImGui::Begin("FiveM Kernel Cheat", &showMenu, ImGuiWindowFlags_NoCollapse);

        if (ImGui::CollapsingHeader("ESP", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Enable ESP", &espEnabled);
            ImGui::Checkbox("Boxes", &espBoxes);
            ImGui::Checkbox("Skeleton", &espSkeleton);
            ImGui::Checkbox("Distance", &espDistance);
            ImGui::Checkbox("Health Bar", &espHealth);
        }

        if (ImGui::CollapsingHeader("Aimbot", ImGuiTreeNodeFlags_DefaultOpen)) {
            ImGui::Checkbox("Enable Aimbot", &aimbotEnabled);
            ImGui::SliderFloat("FOV", &aimbotFOV, 10.0f, 180.0f);
            ImGui::SliderFloat("Smoothing", &aimbotSmooth, 1.0f, 20.0f);

            const char* keys[] = { "Left Mouse", "Right Mouse", "Middle Mouse", "Shift", "Ctrl" };
            static int currentKey = 1;
            if (ImGui::Combo("Aim Key", &currentKey, keys, IM_ARRAYSIZE(keys))) {
                switch (currentKey) {
                    case 0: aimbotKey = VK_LBUTTON; break;
                    case 1: aimbotKey = VK_RBUTTON; break;
                    case 2: aimbotKey = VK_MBUTTON; break;
                    case 3: aimbotKey = VK_SHIFT; break;
                    case 4: aimbotKey = VK_CONTROL; break;
                }
            }
        }

        if (ImGui::CollapsingHeader("Player", ImGuiTreeNodeFlags_DefaultOpen)) {
            if (ImGui::Checkbox("Godmode", &godmodeEnabled)) {
                uintptr_t localPlayer = game->GetLocalPlayer();
                if (localPlayer) {
                    game->SetEntityGodmode(localPlayer, godmodeEnabled);
                }
            }

            if (ImGui::Checkbox("Infinite Ammo", &infiniteAmmoEnabled)) {
                game->SetInfiniteAmmo(infiniteAmmoEnabled);
            }

            if (ImGui::Checkbox("No Recoil", &noRecoilEnabled)) {
                game->SetNoRecoil(noRecoilEnabled);
            }

            ImGui::Checkbox("Super Speed", &superSpeedEnabled);

            if (ImGui::Button("Clear Wanted Level")) {
                game->SetWantedLevel(0);
            }

            if (ImGui::Button("Teleport to Waypoint")) {
                game->TeleportToWaypoint();
            }

            if (ImGui::Button("Heal")) {
                uintptr_t localPlayer = game->GetLocalPlayer();
                if (localPlayer) {
                    game->SetEntityHealth(localPlayer, 200.0f);
                }
            }
        }

        ImGui::End();
    }

    void Render() {
        MSG msg;
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if (msg.message == WM_QUIT) {
                return;
            }
        }

        // Handle menu toggle
        if (GetAsyncKeyState(VK_INSERT) & 1) {
            showMenu = !showMenu;
        }

        // Process features
        ProcessAimbot();

        if (superSpeedEnabled) {
            game->SetSuperSpeed(true);
        }

        if (infiniteAmmoEnabled) {
            game->SetInfiniteAmmo(true);
        }

        if (noRecoilEnabled) {
            game->SetNoRecoil(true);
        }

        // Start ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        // Draw ESP
        ImDrawList* drawList = ImGui::GetBackgroundDrawList();
        DrawESP(drawList);

        // Draw menu
        DrawMenu();

        // Render
        ImGui::Render();

        const float clearColor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
        deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
        deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        swapChain->Present(1, 0);
    }

    void Shutdown() {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();

        CleanupRenderTarget();
        if (swapChain) { swapChain->Release(); swapChain = nullptr; }
        if (deviceContext) { deviceContext->Release(); deviceContext = nullptr; }
        if (device) { device->Release(); device = nullptr; }

        DestroyWindow(hwnd);
        UnregisterClassW(wc.lpszClassName, wc.hInstance);
    }

    static LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg) {
            case WM_SIZE:
                return 0;
            case WM_SYSCOMMAND:
                if ((wParam & 0xfff0) == SC_KEYMENU)
                    return 0;
                break;
            case WM_DESTROY:
                PostQuitMessage(0);
                return 0;
        }
        return DefWindowProcW(hWnd, msg, wParam, lParam);
    }

    bool ShouldRun() {
        return IsWindow(hwnd);
    }
};

struct Vector4 {
    float x, y, z, w;
};
