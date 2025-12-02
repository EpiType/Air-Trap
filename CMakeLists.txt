cmake_minimum_required(VERSION 3.20)
project(AirTrap VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-Wall -Wextra -Werror)

# Find dependencies
find_package(asio REQUIRED)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

# Common library (shared between client and server)
add_library(air-trap-common INTERFACE)
target_include_directories(air-trap-common INTERFACE
    ${CMAKE_SOURCE_DIR}/common/include
)
target_compile_features(air-trap-common INTERFACE cxx_std_23)

# Server executable
add_executable(r-type_server
    server/src/main.cpp
    # Add more source files as needed
)

target_include_directories(r-type_server PRIVATE
    ${CMAKE_SOURCE_DIR}/server/include
)

target_link_libraries(r-type_server PRIVATE
    air-trap-common
    asio::asio
)

# Platform-specific settings for server
if(WIN32)
    target_link_libraries(r-type_server PRIVATE ws2_32)
endif()

# Client executable
add_executable(r-type_client
    client/src/main.cpp
    # Add more source files as needed
)

target_include_directories(r-type_client PRIVATE
    ${CMAKE_SOURCE_DIR}/client/include
)

target_link_libraries(r-type_client PRIVATE
    air-trap-common
    asio::asio
    SFML::Graphics
    SFML::Audio
    SFML::Window
    SFML::System
)

# Platform-specific settings for client
if(WIN32)
    target_link_libraries(r-type_client PRIVATE ws2_32)
    # Create .exe with console
    set_target_properties(r-type_client PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

if(APPLE)
    set_target_properties(r-type_client PROPERTIES
        MACOSX_RPATH TRUE
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

# Installation rules
install(TARGETS r-type_server r-type_client
    RUNTIME DESTINATION bin
)

# Testing (GTest)
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
    add_subdirectory(tests)
endif()


# TO ADD TO THE CURRENT CMakeLists.txt

##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration file, root level
##

cmake_minimum_required(VERSION 3.20)

project(RType VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)
