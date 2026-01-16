cmake_minimum_required(VERSION 3.20)
project(new_rtype LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Reuse dependency config files from the main repo root (asio/SFML/etc).
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/..")

add_subdirectory(engine)
add_subdirectory(plugins/graphical/sfml)
add_subdirectory(plugins/graphical/opengl)
add_subdirectory(plugins/script_lua)
add_subdirectory(games/rtype)
add_subdirectory(games/mario)
add_subdirectory(server)
add_subdirectory(runtime)
