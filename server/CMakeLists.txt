##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Server Binary
##
# Air-Trap/server/CMakeLists.txt

add_executable(r-type_server
    src/main.cpp
    src/Network/Network.cpp
    src/Game/GameManager.cpp
    src/Game/Player.cpp
    src/Game/Room.cpp
)

target_include_directories(r-type_server PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Network
    ${CMAKE_CURRENT_SOURCE_DIR}/include/Game
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
