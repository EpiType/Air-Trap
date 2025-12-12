##
## EPITECH PROJECT, 2025
## R-Type
## File description:
## CMakeLists.txt for Client Plugins
##

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/libs)

macro(create_plugin NAME SOURCE)
    add_library(${NAME} SHARED ${SOURCE})

    target_link_libraries(${NAME} PRIVATE RTypeCommon)
    
    target_compile_features(${NAME})
    
    # Remove the 'lib' prefix on Unix-like systems
    set_target_properties(${NAME} PROPERTIES PREFIX "")

    if(NOT MSVC)
        target_compile_options(${NAME} PRIVATE -Wall -Wextra)
    endif()
    
    message(STATUS "Plugin Configurated : ${NAME}")
endmacro()

# --- PLUGINS LIST ---

find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

# C'est ici que tu ajoutes ton Renderer
# ça va creer une librairie partagée nommée RenderSFML
# grace à ce nom de target tu vas pouvoir ajouter tes dépendances comme ci-dessous

# create_plugin(RenderSFML "Graphics/SFMLRenderer.cpp")

# target_link_libraries(RenderSFML
#     PRIVATE SFML::Graphics
#             SFML::Window
#             SFML::System)

# create_plugin(Boss "Boss.cpp")
