add_library(net_asio SHARED)

target_sources(net_asio PRIVATE
    src/AsioEngine.cpp
)

target_include_directories(net_asio PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/engine/include
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(net_asio PRIVATE
    engine_core
)

set_target_properties(net_asio PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins
)

if(UNIX)
    set_target_properties(net_asio PROPERTIES
        BUILD_RPATH "$ORIGIN/.."
    )
endif()
