##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt for Client
##

file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS "src/*.cpp")

file(GLOB_RECURSE COMMON_SOURCES CONFIGURE_DEPENDS "../common/src/*.cpp")

add_executable(r-type_client
    ${CLIENT_SOURCES}
    ${COMMON_SOURCES}
)

target_include_directories(r-type_client PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/../common/include
)

target_link_libraries(r-type_client PRIVATE
    asio::asio
    SFML::Graphics
    SFML::Audio
    SFML::Window
    SFML::System
)

if(WIN32)
    target_link_libraries(r-type_client PRIVATE ws2_32)
    set_target_properties(r-type_client PROPERTIES WIN32_EXECUTABLE FALSE)
endif()

if(APPLE)
    set_target_properties(r-type_client PROPERTIES MACOSX_RPATH TRUE)
endif()

install(TARGETS r-type_client RUNTIME DESTINATION bin)