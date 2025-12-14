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
};
}  // namespace Client::Core