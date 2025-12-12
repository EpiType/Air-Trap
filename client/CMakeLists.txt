##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Client Binary
##

add_executable(r-type_client
    src/main.cpp
)

target_include_directories(r-type_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(r-type_client PRIVATE
    air-trap-common
    asio::asio
    SFML::Graphics
    SFML::Audio
    SFML::Window
    SFML::System
)

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

install(TARGETS r-type_client
    RUNTIME DESTINATION bin
)
