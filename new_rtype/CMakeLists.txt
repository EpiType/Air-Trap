cmake_minimum_required(VERSION 3.20)
project(new_rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Reuse dependency config files from the main repo root (asio/SFML/etc).
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/..")

add_subdirectory(engine)
add_subdirectory(plugins/net/asio)
add_subdirectory(plugins/graphical/sfml)
add_subdirectory(plugins/graphical/opengl)
add_subdirectory(plugins/script_lua)
add_subdirectory(games/rtype)
add_subdirectory(runtime)

file(MAKE_DIRECTORY "${CMAKE_BINARY_DIR}/engine")
configure_file("${CMAKE_SOURCE_DIR}/engine/config.json"
               "${CMAKE_BINARY_DIR}/engine/config.json" COPYONLY)
file(COPY "${CMAKE_SOURCE_DIR}/assets"
     DESTINATION "${CMAKE_BINARY_DIR}")

