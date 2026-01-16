add_library(engine_core STATIC)
set_target_properties(engine_core PROPERTIES POSITION_INDEPENDENT_CODE ON)

target_sources(engine_core PRIVATE
    src/core/Core.cpp
    src/core/Error.cpp
    src/core/Logger.cpp
    src/core/Time.cpp
    src/ecs/Registry.cpp
    src/ecs/SystemManager.cpp
    src/input/Input.cpp
    src/render/RenderSystem.cpp
    src/audio/AudioSystem.cpp
    src/resource/AssetManager.cpp
    src/net/Network.cpp
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
