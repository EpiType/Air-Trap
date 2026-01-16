# add_library(script_lua SHARED)

# target_sources(script_lua PRIVATE
#     src/ScriptLua.cpp
# )

# target_include_directories(script_lua PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
#     ${CMAKE_SOURCE_DIR}/engine/include
# )

# target_link_libraries(script_lua PRIVATE engine_core)

# set_target_properties(script_lua PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime/plugins
#     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime/plugins
# )
