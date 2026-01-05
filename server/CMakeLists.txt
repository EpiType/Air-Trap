##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Server Binary
##
# Air-Trap/server/CMakeLists.txt

add_executable(r-type_server
    src/main.cpp
    src/ServerNetwork/ServerNetwork.cpp
    src/Game/GameManager.cpp
    src/Game/Player.cpp
    src/Game/Room.cpp
    src/Systems/MovementSystem.cpp
    src/Systems/ServerNetworkSystem.cpp
    src/Systems/AuthSystem.cpp
    src/Systems/RoomSystem.cpp
    src/Systems/PlayerSystem.cpp
    src/Systems/EntitySystem.cpp
)

target_include_directories(r-type_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/ServerNetwork
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Game
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Systems
)

target_link_libraries(r-type_server PRIVATE
    RTypeCommon
)

if(WIN32)
    target_link_libraries(r-type_server PRIVATE ws2_32)
endif()

install(TARGETS r-type_server 
    RUNTIME DESTINATION bin
)

# --- COPY CONFIG ---
# Copy config folder to the build directory (for server settings)
add_custom_command(TARGET r-type_server POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        $<TARGET_FILE_DIR:r-type_server>/config
    COMMENT "Copying config folder to server build directory"
)
