/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** RenderSystem
*/

#pragma once

#include <SFML/Graphics.hpp>

#include "Graphics/AssetManager.hpp"

namespace Client::Systems {
/**
 * @class RenderSystem
 * @brief System responsible for rendering entities.
 *
 * Placeholder for now, will be integrated with ECS later.
 */
class RenderSystem {
   public:
    RenderSystem() = default;

    /**
     * @brief Update and render (placeholder).
     */
    void update(sf::RenderWindow& window, sf::Time delta) {
        (void)window;
        (void)delta;
        // TODO: Integrate with ECS Registry
    }
};
}  // namespace Client::Systems