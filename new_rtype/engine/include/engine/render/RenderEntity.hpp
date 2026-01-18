/**
 * File   : RenderEntity.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_ENGINE_RENDER_RENDERENTITY_HPP_
    #define RTYPE_ENGINE_RENDER_RENDERENTITY_HPP_

    #include <cstdint>
    #include <string>
    #include "engine/math/Vec2.hpp"

/**
 * @namespace aer::render
 * @brief Rendering components and structures for the engine
 */
namespace aer::render
{
    /** 
     * @struct RenderEntity
     * @brief Represents a renderable entity with its properties.
     */
    struct RenderSprite {
        std::uint32_t textureId{0};         /**< Identifier for the texture to use */
        math::Vec2f position{0.0f, 0.0f};   /**< Position of the entity */
        math::Vec2f scale{1.0f, 1.0f};      /**< Scale factor in the Y direction */
        math::Vec2f origin{0.0f, 0.0f};     /**< Origin point for transformations */
        math::Vec2f srcOffset{0.0f, 0.0f};  /**< Offset in the source texture */
        math::Vec2f srcSize{0.0f, 0.0f};    /**< Size of the source texture area */
        float rotation{0.0f};               /**< Rotation of the entity in degrees */
        float r{1.0f};                      /**< Red color component (0.0 to 1.0) */
        float g{1.0f};                      /**< Green color component (0.0 to 1.0) */
        float b{1.0f};                      /**< Blue color component (0.0 to 1.0) */
        float a{1.0f};                      /**< Alpha (transparency) component (0.0 to 1.0) */
        int z{0};                           /**< Z-order for rendering (higher values drawn on top) */
    };

    /** 
     * @struct RenderText
     * @brief Represents a text element to be rendered.
     */
    struct RenderText {
        std::string content;                /**< Text content to render */
        std::uint32_t fontId{0};            /**< Identifier for the font to use */
        math::Vec2f position{0.0f, 0.0f};   /**< Position of the text */
        math::Vec2f scale{1.0f, 1.0f};      /**< Scale factor in the Y direction */
        int size{16};                       /**< Font size in pixels */
        bool bold{false};                   /**< Bold text style */
        bool italic{false};                 /**< Italic text style */
        float r{1.0f};                      /**< Red color component (0.0 to 1.0) */
        float g{1.0f};                      /**< Green color component (0.0 to 1.0) */
        float b{1.0f};                      /**< Blue color component (0.0 to 1.0) */
        float a{1.0f};                      /**< Alpha (transparency) component (0.0 to 1.0) */
        int z{0};                           /**< Z-order for rendering (higher values drawn on top) */
    };

    /** 
     * @struct RenderRect
     * @brief Represents a colored rectangle to be rendered.
     */
    struct RenderRect {
        math::Vec2f position{0.0f, 0.0f};   /**< Position of the rectangle */
        math::Vec2f size{100.0f, 100.0f};   /**< Size of the rectangle */
        bool filled{true};                  /**< Filled rectangle or outline */
        float thickness{1.0f};              /**< Outline thickness when not filled */
        float r{1.0f};                      /**< Red color component (0.0 to 1.0) */
        float g{1.0f};                      /**< Green color component (0.0 to 1.0) */
        float b{1.0f};                      /**< Blue color component (0.0 to 1.0) */
        float a{1.0f};                      /**< Alpha (transparency) component (0.0 to 1.0) */
        int z{0};                           /**< Z-order for rendering (higher values drawn on top) */
    };
}

#endif /* !RTYPE_ENGINE_RENDER_RENDERENTITY_HPP_ */
