add_library(engine_core SHARED)
set_target_properties(engine_core PROPERTIES
    POSITION_INDEPENDENT_CODE ON
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_sources(engine_core PRIVATE
    src/core/Core.cpp
    src/log/Error.cpp
    src/log/Logger.cpp
    src/core/Time.cpp
    src/ecs/Registry.cpp
    src/ecs/SystemManager.cpp
    src/input/Input.cpp
    src/audio/AudioSystem.cpp
    src/net/ANetwork.cpp
    src/ui/UiFactory.cpp
    src/plugin/DynamicLibrary.cpp
    src/plugin/LibraryManager.cpp
)

if (WIN32)
    target_sources(engine_core PRIVATE
        src/plugin/LibraryBackend_Win.cpp
    )
else()
    target_sources(engine_core PRIVATE
        src/plugin/LibraryBackend_Unix.cpp
    )
endif()

target_include_directories(engine_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_definitions(engine_core PUBLIC ASIO_STANDALONE)

find_package(asio REQUIRED)

target_link_libraries(engine_core
    PUBLIC
        asio::asio
)

if(UNIX)
    target_link_libraries(engine_core PRIVATE ${CMAKE_DL_LIBS})
endif()

add_executable(rtype_engine
    src/main.cpp
)

target_include_directories(rtype_engine PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(rtype_engine PRIVATE
    engine_core
)

set_target_properties(rtype_engine PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

if(UNIX)
    target_link_options(rtype_engine PRIVATE "-Wl,-rpath,$ORIGIN")
endif()
