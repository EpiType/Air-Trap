##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Common Binary
##

add_library(air-trap-common INTERFACE)

target_include_directories(air-trap-common INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_features(air-trap-common INTERFACE cxx_std_23)