/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Application
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <memory>
#include <optional>
#include <vector>
#include <string>

#include "Graphics/AssetManager.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Game/EntityBuilder.hpp"

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

enum class GameState {
    Menu,
    Playing,
    Settings,
    Paused
};

class Application {
public:
    Application();
    void run();
    
    private:
    void initECS();
    void initMenu();
    void initGame();
    
    void processInput();
    void update(sf::Time delta);
    void render();
    
    void changeState(GameState newState);
    void spawnEnemy(const rtp::Vec2f& position);
    void spawnProjectile(const rtp::Vec2f& position);
    void killEnemy(std::size_t index);
    void killProjectile(std::size_t index);
    
    std::vector<rtp::ecs::Entity> _spawnedEnemy;
    std::vector<rtp::ecs::Entity> _projectiles;
    sf::RenderWindow _window;
    rtp::ecs::Registry _registry;
    rtp::ecs::SystemManager _systemManager;
    Graphics::AssetManager _assetManager;
    Game::EntityBuilder _entityBuilder;
    
    GameState _currentState{GameState::Menu};
    float _lastDt{0.0f};
};

}  // namespace Client::Core