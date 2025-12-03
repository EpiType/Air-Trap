/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** RenderSystem
*/

#pragma once

#include <SFML/Graphics.hpp>

#include "Client/Graphics/AssetManager.hpp"
#include "Client/Systems/RenderSystem.hpp"
#include "RType/ECS/Components/Position.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Coordinator.hpp"
#include "RType/ECS/System/System.hpp"

/* Components */
#include "RType/ECS/Components/Position.hpp"
#include "RType/ECS/Components/Sprite.hpp"

namespace Client::Systems {
/**
 * @class RenderSystem
 * @brief System responsible for rendering entities with Position and Sprite components.
 *
 * This system iterates over all entities that have both Position and Sprite components
 * and draws their associated sprites at the specified positions on the render target.
 */
class RenderSystem : public rtp::ecs::System {
   public:
    RenderSystem(sf::RenderWindow& window) : _window(window) {}

    /**
     * @brief Update and render all entities managed by this system.
     * @param coordinator Reference to the ECS coordinator for component access.
     * @param window Reference to the SFML render window.
     * @param assetManager Reference to the asset manager for resource access.
     * @param delta Time elapsed since the last update.
     */
    void update(rtp::ecs::Coordinator& coordinator, sf::RenderWindow& window, Graphics::AssetManager& assetManager,
                sf::Time delta) {
        for (auto const& entity : this->_Entities) {
            auto& position = coordinator.getComponent<rtp::ecs::components::Position>(entity);
            auto& spriteComp = coordinator.getComponent<rtp::ecs::components::Sprite>(entity);

            spriteComp.sprite.setPosition(sf::Vector2f(position.x, position.y));
            window.draw(spriteComp.sprite);
        }
    }

   private:
    sf::RenderWindow& _window;
};
}  // namespace Client::Systems