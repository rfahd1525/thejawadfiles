cmake_minimum_required(VERSION 3.15)
project(FiveMKernelCheat)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add ImGui sources
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/imgui)
file(GLOB IMGUI_SOURCES
    ${IMGUI_DIR}/*.cpp
    ${IMGUI_DIR}/backends/imgui_impl_dx11.cpp
    ${IMGUI_DIR}/backends/imgui_impl_win32.cpp
)

# Client executable
add_executable(FiveMCheat
    main.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(FiveMCheat PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IMGUI_DIR}
    ${IMGUI_DIR}/backends
    ../shared
)

# Link libraries
target_link_libraries(FiveMCheat
    d3d11
    dxgi
    dwmapi
)

# Set Windows subsystem
set_target_properties(FiveMCheat PROPERTIES
    WIN32_EXECUTABLE FALSE
    LINK_FLAGS "/SUBSYSTEM:CONSOLE"
)

# Copy DLLs if needed
if(CMAKE_BUILD_TYPE MATCHES Debug)
    set_target_properties(FiveMCheat PROPERTIES
        OUTPUT_NAME "FiveMCheat_Debug"
    )
endif()
