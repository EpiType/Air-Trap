# add_library(render_opengl SHARED)

# target_sources(render_opengl PRIVATE
#     src/RenderOpenGL.cpp
# )

# target_include_directories(render_opengl PRIVATE
#     ${CMAKE_CURRENT_SOURCE_DIR}/include
#     ${CMAKE_SOURCE_DIR}/engine/include
# )

# target_link_libraries(render_opengl PRIVATE engine_core)

# set_target_properties(render_opengl PROPERTIES
#     RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime/plugins
#     LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/runtime/plugins
# )
