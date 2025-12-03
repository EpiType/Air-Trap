/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Application
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <string>

#include "Client/Graphics/AssetManager.hpp"
#include "Client/Systems/InputSystem.hpp"
#include "Client/Systems/RenderSystem.hpp"
#include "RType/ECS/Components/Position.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Coordinator.hpp"

namespace Client::Core {
/**
 * @class Application
 * @brief Main application class for the R-Type client.
 *
 * This class initializes and runs the main application loop,
 * handling window creation, ECS setup, and system management.
 */
class Application {
   public:
    Application();
    void run();

   private:
    void initECS();
    void processInput();
    void update(sf::Time delta);
    void render();

    sf::RenderWindow _window;

    std::unique_ptr<rtp::ecs::Coordinator> _coordinator;

    Graphics::AssetManager _assetManager;

    std::shared_ptr<rtp::ecs::System> _renderSystem;
    std::shared_ptr<rtp::ecs::System> _inputSystem;
};
}  // namespace Client::Core