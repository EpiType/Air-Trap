/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** InputSystem
*/

#pragma once

#include <SFML/Window.hpp>

namespace Client::Systems {
/**
 * @class InputSystem
 * @brief System responsible for handling player input.
 *
 * Placeholder for now, will be integrated with ECS later.
 */
class InputSystem {
   public:
    InputSystem() = default;

    /**
     * @brief Process input events (placeholder).
     */
    void update(sf::Event& event) {
        (void)event;
        // TODO: Integrate with ECS Registry
    }
};
}  // namespace Client::Systems