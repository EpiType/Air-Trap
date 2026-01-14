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
    src/Network/ClientNetwork.cpp
    src/Core/Settings.cpp
    src/Translation/TranslationManager.cpp
    src/Systems/UISystem.cpp
    src/UI/UiFactory.cpp
    src/Systems/UIRenderSystem.cpp
    src/Systems/RenderSystem.cpp
    src/Game/EntityBuilder.cpp
    # src/Systems/SettingsMenuSystem.cpp
    # src/Systems/MovementSystem.cpp
    src/Systems/InputSystem.cpp
    src/Systems/NetworkSyncSystem.cpp
    # Scenes
    src/Scenes/CreateRoomScene.cpp
    src/Scenes/GamepadSettingsScene.cpp
    src/Scenes/KeyBindingScene.cpp
    src/Scenes/LoginScene.cpp
    src/Scenes/LobbyScene.cpp
    src/Scenes/MenuScene.cpp
    src/Scenes/PauseScene.cpp
    src/Scenes/RoomWaitingScene.cpp
    src/Scenes/SettingsScene.cpp
    src/Scenes/PlayingScene.cpp
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

# --- DEBUG MODE ---
# Enable DEBUG flag in Debug and RelWithDebInfo builds
if(CMAKE_BUILD_TYPE MATCHES "Debug|RelWithDebInfo")
    target_compile_definitions(r-type_client PRIVATE DEBUG)
    message(STATUS "Client: DEBUG mode enabled (debug keys J/K/L/M available)")
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

# --- COPY ASSETS ---
# Copy assets and config folders to the build directory
add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        $<TARGET_FILE_DIR:r-type_client>/assets
    COMMENT "Copying assets folder to build directory"
)

add_custom_command(TARGET r-type_client POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        $<TARGET_FILE_DIR:r-type_client>/config
    COMMENT "Copying config folder to build directory"
)
