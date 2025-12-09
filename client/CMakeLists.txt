##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Client Binary
##

# Find SFML (Conan)
find_package(SFML REQUIRED CONFIG COMPONENTS Graphics Window System Audio)
find_package(asio REQUIRED CONFIG)

# Client executable
add_executable(r-type_client
    src/main.cpp
    src/Core/Application.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CMAKE_SOURCE_DIR}/client/include
    ${CMAKE_SOURCE_DIR}/common/include
)

target_link_libraries(r-type_client PRIVATE
    RTypeCommon
    asio::asio
    SFML::Graphics
    SFML::Audio
    SFML::Window
    SFML::System
)

# Platform-specific settings
if(WIN32)
    target_link_libraries(r-type_client PRIVATE ws2_32)
    set_target_properties(r-type_client PROPERTIES
        WIN32_EXECUTABLE FALSE
    )
endif()

if(APPLE)
    set_target_properties(r-type_client PROPERTIES
        MACOSX_RPATH TRUE
    )
endif()
