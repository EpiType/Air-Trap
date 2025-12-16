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
    src/Core/Settings.cpp
    src/Translation/TranslationManager.cpp
    src/Systems/MenuSystem.cpp
    src/Systems/UIRenderSystem.cpp
    src/Systems/RenderSystem.cpp
    src/Game/EnemyBuilder.cpp
    src/Systems/SettingsMenuSystem.cpp
    # src/Systems/MovementSystem.cpp
    # src/Systems/InputSystem.cpp
)

# --- TARGET ---
add_executable(r-type_client ${SRC_CLIENT})

# --- INCLUDES ---
target_include_directories(r-type_client
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Network
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Graphics
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Systems
        ${CMAKE_CURRENT_SOURCE_DIR}/include/Input
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
