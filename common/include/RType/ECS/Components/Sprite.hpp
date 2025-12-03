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
    std::string textureId{"0"}; /**< Identifier for the texture resource */
    sf::IntRect textureRect;    /**< Rectangle defining the texture area */
    int zIndex{0};              /**< Z-index for rendering order */
    sf::Sprite sprite;          /**< SFML Sprite object */
};
}  // namespace rtp::ecs::components