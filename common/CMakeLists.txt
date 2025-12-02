##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt, CMake configuration for Common Library
##

# Dynamic Libraries sources
set(SRC_DYNLIB
    src/Core/DynamicLibraries/LibraryManager.cpp
    src/Core/DynamicLibraries/DynamicLibrary.cpp
)

# Platform-specific source files for Dynamic Libraries
if(WIN32)
    list(APPEND SRC_DYNLIB
        src/Core/DynamicLibraries/impl/PlatformBackend_Win.cpp
    )
elseif(UNIX)
    list(APPEND SRC_DYNLIB
        src/Core/DynamicLibraries/impl/PlatformBackend_Unix.cpp
    )
endif()

set(SRC_LOGGER
    src/Core/Logger/Logger.cpp
)

# Create a static library for common components
add_library(RTypeCommon STATIC
    ${SRC_LOGGER}
    ${SRC_DYNLIB}
    ${SRC_NETWORK}
)

# Target Configuration
target_include_directories(RTypeCommon
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

if(UNIX)
    target_link_libraries(RTypeCommon PRIVATE ${CMAKE_DL_LIBS})
endif()
