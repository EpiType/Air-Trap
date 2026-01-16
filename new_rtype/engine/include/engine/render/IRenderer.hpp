/**
 * File   : IRenderer.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_ENGINE_RENDER_IRENDERER_HPP_
    #define RTYPE_ENGINE_RENDER_IRENDERER_HPP_

    #include <cstdint>
    #include <string>
    #include <vector>
    #include "engine/input/Event.hpp"
    #include "engine/render/RenderFrame.hpp"

/**
 * @namespace engine::render
 * @brief Rendering components and structures for the engine
 */
namespace engine::render
{
    /**
     * @class IRenderer
     * @brief Interface for renderer implementations.
     * 
     * This interface defines the contract for renderer plugins that can be
     * loaded into the engine. Implementations must provide methods for
     * initializing the renderer, managing frames, and loading resources.
     */
    class IRenderer {
        public:
            virtual ~IRenderer() = default;

            /** 
             * @brief Initialize the renderer with the specified window parameters.
             * @param width Width of the render window
             * @param height Height of the render window
             * @param title Title of the render window
             * @return true if initialization was successful, false otherwise
             */
            virtual bool init(int width, int height, const std::string& title) = 0;

            /** 
             * @brief Shutdown the renderer and release resources.
             */
            virtual void shutdown(void) = 0;

            /** 
             * @brief Get the name of the renderer implementation.
             * @return Name of the renderer as a string
             */
            virtual std::string getName(void) const = 0;

            /** 
             * @brief Begin rendering a new frame.
             */
            virtual void beginFrame(void) = 0;

            /** 
             * @brief Draw the provided render frame.
             * @param frame RenderFrame object containing entities to draw
             */
            virtual void draw(const RenderFrame& frame) = 0;

            /** 
             * @brief End rendering the current frame.
             */
            virtual void endFrame(void) = 0;

            /** 
             * @brief Resize the render window.
             * @param width New width of the window
             * @param height New height of the window
             */
            virtual void resize(int width, int height) = 0;

            /** 
             * @brief Load a texture from the specified file path.
             * @param path File path to the texture
             * @return Unique identifier for the loaded texture
             */
            virtual std::uint32_t loadTexture(const std::string& path) = 0;

            /** 
             * @brief Unload a previously loaded texture.
             * @param id Unique identifier of the texture to unload
             */
            virtual void unloadTexture(std::uint32_t id) = 0;

            /** 
             * @brief Load a font from the specified file path.
             * @param path File path to the font
             * @return Unique identifier for the loaded font
             */
            virtual std::uint32_t loadFont(const std::string& path) = 0;

            /** 
             * @brief Unload a previously loaded font.
             * @param id Unique identifier of the font to unload
             */
            virtual void unloadFont(std::uint32_t id) = 0;

            /** 
             * @brief Poll and process window events.
             * @param outEvents Output vector to append events into
             * @return true if the window is still open, false otherwise
             */
            virtual bool pollEvents(std::vector<input::Event>& outEvents) = 0;
    };
}

#endif /* !RTYPE_ENGINE_RENDER_IRENDERER_HPP_ */
