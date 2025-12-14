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

#include "Graphics/AssetManager.hpp"
#include "Systems/InputSystem.hpp"

#include "RType/ECS/Registry.hpp"
#include "RType/ECS/SystemManager.hpp"

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

    Graphics::AssetManager _assetManager;
    rtp::ecs::Registry _registry;
    rtp::ecs::SystemManager _systemManager;

    std::unique_ptr<rtp::client::RenderSystem> _renderSystem;
    float _lastDt{0.0f};
};
}  // namespace Client::Core