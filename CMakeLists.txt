cmake_minimum_required(VERSION 3.20)
project(AirTrap VERSION 1.0.0 LANGUAGES CXX)

# --- GLOBAL CONFIGURATION ---
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Werror)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# --- EXTERNAL DEPENDENCIES ---
find_package(asio REQUIRED)
find_package(SFML 3 REQUIRED COMPONENTS Graphics Window System Audio)

# Testing (GTest)
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)
endif()

# --- SUBDIRECTORIES ---
add_subdirectory(common)
add_subdirectory(server)
add_subdirectory(client)

if(BUILD_TESTS)
    add_subdirectory(tests)
endif()