add_executable(engine_runtime)

target_sources(engine_runtime PRIVATE
    src/main.cpp
)

target_include_directories(engine_runtime PRIVATE
    ${CMAKE_SOURCE_DIR}/engine/include
)

target_link_libraries(engine_runtime PRIVATE
    engine_core
)

set_target_properties(engine_runtime PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime
)
