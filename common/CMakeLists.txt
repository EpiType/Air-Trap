##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Common Library
##

cmake_minimum_required(VERSION 3.20)

set(SRC_DYNLIB
    src/Core/DynamicLibraries/LibraryManager.cpp
    src/Core/DynamicLibraries/DynamicLibrary.cpp
)

if(WIN32)
    list(APPEND SRC_DYNLIB src/Core/DynamicLibraries/impl/PlatformBackend_Win.cpp)
elseif(UNIX)
    list(APPEND SRC_DYNLIB src/Core/DynamicLibraries/impl/PlatformBackend_Unix.cpp)
endif()

set(SRC_LOGGER
    src/Core/Logger/Logger.cpp
)

# set(SRC_ECS
#     src/RType/ECS/Registry.cpp
# )

add_library(RTypeCommon STATIC
    ${SRC_LOGGER}
    ${SRC_DYNLIB}
    # ${SRC_ECS}
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