/**
 * File   : SpritePreview.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 14/01/2026
 */

#ifndef RTYPE_ECS_COMPONENTS_UI_SPRITEPREVIEW_HPP_
#define RTYPE_ECS_COMPONENTS_UI_SPRITEPREVIEW_HPP_

#include <string>

namespace rtp::ecs::components::ui {

    /**
     * @struct SpritePreview
     * @brief Component for displaying a sprite preview in the UI.
     * 
     * This component holds information about a sprite to display,
     * including texture path and rectangle coordinates for extracting
     * a specific portion of the texture (for sprite sheets).
     */
    struct SpritePreview {
        std::string texturePath;    /**< Path to the texture file */
        float x{0.0f};              /**< X position for rendering */
        float y{0.0f};              /**< Y position for rendering */
        float scale{1.0f};          /**< Scale factor for display */
        
        // Texture rectangle for sprite sheet extraction
        int rectLeft{0};            /**< Left coordinate in texture */
        int rectTop{0};             /**< Top coordinate in texture */
        int rectWidth{0};           /**< Width of sprite in texture */
        int rectHeight{0};          /**< Height of sprite in texture */
        
        int zIndex{0};              /**< Rendering order */
    };

} // namespace rtp::ecs::components::ui

#endif // RTYPE_ECS_COMPONENTS_UI_SPRITEPREVIEW_HPP_
