set(RTYPE_SHARED_SOURCES
    shared/src/Packet.cpp
)

set(RTYPE_CLIENT_SOURCES
    client/src/ClientApp.cpp
    client/src/utility/Settings.cpp
    client/src/utility/TranslationManager.cpp
    client/src/entity/EntityBuilder.cpp
    client/src/net/NetworkBootstrap.cpp
    client/src/scenes/LoginScene.cpp
    client/src/scenes/MenuScene.cpp
    client/src/scenes/LobbyScene.cpp
    client/src/scenes/CreateRoomScene.cpp
    client/src/scenes/RoomWaitingScene.cpp
    client/src/scenes/PlayingScene.cpp
    client/src/scenes/PauseScene.cpp
    client/src/scenes/SettingsScene.cpp
    client/src/scenes/KeyBindingScene.cpp
    client/src/scenes/GamepadSettingsScene.cpp
    client/src/systems/AnimationSystem.cpp
    client/src/systems/InputSystem.cpp
    client/src/systems/NetworkSyncSystem.cpp
    client/src/systems/ParallaxSystem.cpp
    client/src/systems/RenderSystem.cpp
    client/src/systems/UISystem.cpp
    client/src/systems/UIRenderSystem.cpp
)

set(RTYPE_SERVER_SOURCES
    server/src/ServerApp.cpp
    server/src/game/GameManager.cpp
    server/src/game/Player.cpp
    server/src/game/Room.cpp
    server/src/net/NetworkBootstrap.cpp
    server/src/systems/AuthSystem.cpp
    server/src/systems/PlayerSystem.cpp
    server/src/systems/RoomSystem.cpp
)

set(RTYPE_INCLUDE_DIRS
    ${CMAKE_CURRENT_SOURCE_DIR}/client/include
    ${CMAKE_CURRENT_SOURCE_DIR}/server/include
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/include
    ${CMAKE_SOURCE_DIR}/engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

add_library(rtype_client_plugin SHARED
    ${RTYPE_SHARED_SOURCES}
    ${RTYPE_CLIENT_SOURCES}
)

target_include_directories(rtype_client_plugin PRIVATE
    ${RTYPE_INCLUDE_DIRS}
)

target_link_libraries(rtype_client_plugin PRIVATE
    engine_core
)

set_target_properties(rtype_client_plugin PROPERTIES
    OUTPUT_NAME client
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/games/rtype
)

add_library(rtype_server_plugin SHARED
    ${RTYPE_SHARED_SOURCES}
    ${RTYPE_SERVER_SOURCES}
)

target_include_directories(rtype_server_plugin PRIVATE
    ${RTYPE_INCLUDE_DIRS}
)

target_link_libraries(rtype_server_plugin PRIVATE
    engine_core
)

set_target_properties(rtype_server_plugin PROPERTIES
    OUTPUT_NAME server
    PREFIX ""
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/games/rtype
)

if(UNIX)
    set_target_properties(rtype_client_plugin PROPERTIES
        BUILD_RPATH "$ORIGIN/../.."
    )
    set_target_properties(rtype_server_plugin PROPERTIES
        BUILD_RPATH "$ORIGIN/../.."
    )
endif()

add_custom_target(rtype_game_config ALL
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/games/rtype
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${CMAKE_CURRENT_SOURCE_DIR}/config.yml
        ${CMAKE_BINARY_DIR}/games/rtype/config.yml
    COMMENT "Copying game config.yml to build/games/rtype"
)
