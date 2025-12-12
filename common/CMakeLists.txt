##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Common Library
##

cmake_minimum_required(VERSION 3.20)

# --- SOURCES ---

# NOTE: DON'T USE GLOB_RECURSE,
# If you need to add a file, add it MANUALLY here
# If your file doesn't belong to any category, create a new variable for it.

set(SRC_DYNLIB
    src/System/LibraryManager.cpp
    src/System/DynamicLibrary.cpp
)
if(WIN32)
    list(APPEND SRC_DYNLIB src/System/LibraryBackend_Win.cpp)
elseif(UNIX)
    list(APPEND SRC_DYNLIB src/System/LibraryBackend_Unix.cpp)
endif()

set(SRC_LOGGER
    src/Logger.cpp
)

set(SRC_THREADPOOL
    src/Thread/ThreadPool.cpp
)

set(SRC_ERROR
    src/Error.cpp
)

set(SRC_ECS
    src/ECS/Registry.cpp
)

# USE OF GLOBAL RECURSE JUST TO COLLECT HEADERS FOR INSTALLATION PURPOSES
file(GLOB_RECURSE PUBLIC_HEADERS "include/*.hpp")

# --- TARGET ---

add_library(RTypeCommon STATIC
    ${SRC_LOGGER}
    ${SRC_DYNLIB}
    ${SRC_ECS}
    ${SRC_THREADPOOL}
    ${SRC_ERROR}

    ${PUBLIC_HEADERS}
)

# --- INCLUDES ---

target_include_directories(RTypeCommon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(RTypeCommon PUBLIC ${CMAKE_CXX_STANDARD})

if(MSVC)
    target_compile_options(RTypeCommon PRIVATE /W4)
else()
    target_compile_options(RTypeCommon PRIVATE -Wall -Wextra -Wpedantic)
endif()

# --- DEPENDENCIES ---

# ASIO WITHOUT BOOST
target_compile_definitions(RTypeCommon PUBLIC ASIO_STANDALONE)

find_package(asio REQUIRED)

target_link_libraries(RTypeCommon 
    PUBLIC 
        asio::asio
)

# LINK DL ON UNIX SYSTEMS
if(UNIX)
    target_link_libraries(RTypeCommon PRIVATE ${CMAKE_DL_LIBS})
endif()
