##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Common Library
##

cmake_minimum_required(VERSION 3.20)

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

add_library(RTypeCommon STATIC
    ${SRC_LOGGER}
    ${SRC_DYNLIB}
    ${SRC_ECS}
    ${SRC_THREADPOOL}
    ${SRC_ERROR}
)

target_include_directories(RTypeCommon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_compile_features(RTypeCommon PUBLIC cxx_std_23)

find_package(asio REQUIRED)

target_link_libraries(RTypeCommon 
    PUBLIC 
        asio::asio
)

if(UNIX)
    target_link_libraries(RTypeCommon PRIVATE ${CMAKE_DL_LIBS})
endif()
