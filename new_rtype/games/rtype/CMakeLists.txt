add_library(rtype_game STATIC)

target_sources(rtype_game PRIVATE
    shared/src/Packet.cpp
    client/src/ClientApp.cpp
    client/src/utility/Settings.cpp
    client/src/utility/TranslationManager.cpp
    client/src/entity/EntityBuilder.cpp
    client/src/net/NetworkBootstrap.cpp
    client/src/scenes/LoginScene.cpp
    client/src/scenes/RegisterScene.cpp
    client/src/scenes/MenuScene.cpp
    client/src/scenes/LobbyScene.cpp
    client/src/systems/AnimationSystem.cpp
    client/src/systems/InputSystem.cpp
    client/src/systems/NetworkSyncSystem.cpp
    client/src/systems/ParallaxSystem.cpp
    client/src/systems/RenderSystem.cpp
    client/src/systems/UISystem.cpp
    client/src/systems/UIRenderSystem.cpp
    server/src/ServerApp.cpp
    server/src/game/GameManager.cpp
    server/src/game/Player.cpp
    server/src/game/Room.cpp
    server/src/net/NetworkBootstrap.cpp
    server/src/systems/AuthSystem.cpp
    server/src/systems/PlayerSystem.cpp
    server/src/systems/RoomSystem.cpp
)

target_include_directories(rtype_game PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/client/include
    ${CMAKE_CURRENT_SOURCE_DIR}/server/include
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/include
    ${CMAKE_SOURCE_DIR}/engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rtype_game PRIVATE engine_core)

add_executable(rtype_client
    client/src/main.cpp
)

target_include_directories(rtype_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/client/include
    ${CMAKE_CURRENT_SOURCE_DIR}/server/include
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/include
    ${CMAKE_SOURCE_DIR}/engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rtype_client PRIVATE
    rtype_game
    engine_core
)

set_target_properties(rtype_client PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_executable(rtype_server
    server/src/main.cpp
)

target_include_directories(rtype_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/client/include
    ${CMAKE_CURRENT_SOURCE_DIR}/server/include
    ${CMAKE_CURRENT_SOURCE_DIR}/shared/include
    ${CMAKE_SOURCE_DIR}/engine/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(rtype_server PRIVATE
    rtype_game
    engine_core
)

set_target_properties(rtype_server PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
