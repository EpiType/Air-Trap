##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, Root CMake configuration
##

cmake_minimum_required(VERSION 3.20)
project(RType VERSION 1.0.0 LANGUAGES CXX)

# C++ Standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Export compile commands for clang-tidy
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Compiler warnings
add_compile_options(-Wall -Wextra -Werror)

# Add subdirectories
add_subdirectory(common)
add_subdirectory(client)
add_subdirectory(server)

# Testing (GTest)
option(BUILD_TESTS "Build unit tests" ON)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

