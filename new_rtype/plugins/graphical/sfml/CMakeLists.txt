add_library(render_sfml SHARED)

target_sources(render_sfml PRIVATE
    src/RenderSFML.cpp
)

target_include_directories(render_sfml PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/include
)

find_package(SFML REQUIRED COMPONENTS graphics window system)

target_link_libraries(render_sfml PRIVATE
    engine_core
    SFML::Graphics
    SFML::Window
    SFML::System
)

set_target_properties(render_sfml PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

if(UNIX)
    set_target_properties(render_sfml PROPERTIES
        BUILD_RPATH "$ORIGIN/.."
    )
endif()
