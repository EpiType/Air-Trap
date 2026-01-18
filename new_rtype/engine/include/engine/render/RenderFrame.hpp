/**
 * File   : RenderFrame.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_ENGINE_RENDER_RENDERFRAME_HPP_
    #define RTYPE_ENGINE_RENDER_RENDERFRAME_HPP_

    #include <vector>
    #include "engine/render/RenderEntity.hpp"

/**
 * @namespace aer::render
 * @brief Rendering components and structures for the engine
 */
namespace aer::render
{
    /**
     * @struct RenderFrame
     * @brief Represents a frame containing multiple renderable entities.
     * 
     * This structure holds a collection of RenderEntity objects that
     * represent the visual elements to be drawn in a single frame.
     */
    struct RenderFrame {
        /**
         * @brief List of renderable entities in the frame.
         */
        std::vector<RenderSprite> sprites;

        /**
         * @brief List of text elements to render in the frame.
         */
        std::vector<RenderText> texts;

        /**
         * @brief List of rectangle shapes to render in the frame.
         */
        std::vector<RenderRect> shapes;

        /**
         * @brief Clear all renderable entities from the frame.
         */
        void clear() { sprites.clear(); texts.clear(); shapes.clear(); }

        /**
         * @brief Check if the frame has no renderable entities.
         * @return true if the frame is empty, false otherwise.
         */
        bool empty() const { return sprites.empty() && texts.empty() && shapes.empty(); }
    };
}

#endif /* !RTYPE_ENGINE_RENDER_RENDERFRAME_HPP_ */