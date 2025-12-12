##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt for Client Executable
##

# --- SOURCES ---
# Add the core sources files here.
set(SRC_CLIENT
    src/main.cpp
    src/Core/Application.cpp
    # src/Input/InputManager.cpp
)

# --- TARGET ---
add_executable(r-type_client ${SRC_CLIENT})

# --- INCLUDES ---
target_include_directories(r-type_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

if(NOT MSVC)
    target_compile_options(r-type_client PRIVATE -Wall -Wextra)
endif()

# --- DEPENDENCIES ---
find_package(SFML REQUIRED COMPONENTS graphics window system)

# --- LINKING ---
target_link_libraries(r-type_client
    PRIVATE
        RTypeCommon
        SFML::Graphics
        SFML::Window
        SFML::System
)

# --- PLUGINS ---
add_subdirectory(plugins)
