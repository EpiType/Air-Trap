##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Server Binary
##

# Find ASIO (Conan)
find_package(asio REQUIRED CONFIG)

# Server executable
add_executable(r-type_server
    src/main.cpp
)

target_include_directories(r-type_server PRIVATE
    ${CMAKE_SOURCE_DIR}/server/include
    ${CMAKE_SOURCE_DIR}/common/include
)

target_link_libraries(r-type_server PRIVATE
    RTypeCommon
    asio::asio
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(r-type_server PRIVATE ws2_32)
endif()
