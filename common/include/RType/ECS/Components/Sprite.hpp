/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Sprite
*/

#pragma once

#include <SFML/Graphics/Rect.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <string>

namespace rtp::ecs::components {
/**
 * @struct Sprite
 * @brief Component representing a sprite.
 *
 * Note: Actual sprite data (e.g., texture, dimensions) should be
 * defined according to the rendering library used (e.g., SFML, SDL).
 */
struct Sprite {
    std::string texturePath{"0"}; /**< Identifier for the texture resource */

    int rectLeft{0};         /**< Left position of the texture rectangle */
    int rectTop{0};          /**< Top position of the texture rectangle */
    int rectWidth{0};        /**< Width of the texture rectangle */
    int rectHeight{0};       /**< Height of the texture rectangle */

    int zIndex{0};           /**< Z-index for rendering order */

    uint8_t opacity{255};    /**< Opacity of the sprite (0-255) */
    uint8_t red{255};        /**< Red color component */
    uint8_t green{255};      /**< Green color component */
    uint8_t blue{255};       /**< Blue color component */
};
}  // namespace rtp::ecs::components