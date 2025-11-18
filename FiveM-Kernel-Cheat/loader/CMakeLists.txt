cmake_minimum_required(VERSION 3.15)
project(DriverLoader)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Loader executable
add_executable(loader loader.cpp)

# Set Windows subsystem
set_target_properties(loader PROPERTIES
    WIN32_EXECUTABLE FALSE
    LINK_FLAGS "/SUBSYSTEM:CONSOLE"
)

# Request admin privileges
if(MSVC)
    set_target_properties(loader PROPERTIES
        LINK_FLAGS "/MANIFESTUAC:\"level='requireAdministrator' uiAccess='false'\" /SUBSYSTEM:CONSOLE"
    )
endif()
