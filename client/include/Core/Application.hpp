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
#include <filesystem>

#include "Graphics/AssetManager.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Game/EntityBuilder.hpp"
#include "Translation/TranslationManager.hpp"
#include "Core/Settings.hpp"

#include "RType/ECS/Registry.hpp"
#include "RType/ECS/SystemManager.hpp"

namespace Client::Core {

// UI Layout Constants
namespace UIConstants {
    // Window
    constexpr float WINDOW_WIDTH = 1280.0f;
    constexpr float WINDOW_HEIGHT = 720.0f;
    
    // Menu Layout
    constexpr float TITLE_X = 400.0f;
    constexpr float TITLE_Y = 100.0f;
    constexpr float TITLE_FONT_SIZE = 72.0f;
    
    constexpr float BUTTON_WIDTH = 300.0f;
    constexpr float BUTTON_HEIGHT = 60.0f;
    constexpr float BUTTON_X = 490.0f;
    constexpr float BUTTON_SPACING = 80.0f;
    constexpr float BUTTON_START_Y = 300.0f;
    
    // Player spawn
    constexpr float PLAYER_SPAWN_X = 200.0f;
    constexpr float PLAYER_SPAWN_Y = 150.0f;
}

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
    KeyBindings,
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
    void initSettingsMenu();
    void initKeyBindingMenu();
    void setupSettingsCallbacks();
    
    void processInput();
    void update(sf::Time delta);
    void render();
    
    void processMenuInput(const sf::Event& event);
    void processGameInput(const sf::Event& event);
    void processSettingsInput(const sf::Event& event);
    void processKeyBindingInput(const sf::Event& event);
    void handleGlobalEscape();
    
    void changeState(GameState newState);
    
    void createParallaxBackground();
    void spawnEnemy(const rtp::Vec2f& position);
    void spawnProjectile(const rtp::Vec2f& position);
    void killEnemy(std::size_t index);
    void killProjectile(std::size_t index);
    
    std::vector<rtp::ecs::Entity> _parallaxLayers;
    std::vector<rtp::ecs::Entity> _spawnedEnemy;
    std::vector<rtp::ecs::Entity> _projectiles;
    sf::RenderWindow _window;
    rtp::ecs::Registry _registry;
    rtp::ecs::SystemManager _systemManager;
    Graphics::AssetManager _assetManager;
    Game::EntityBuilder _entityBuilder;
    Settings _settings;
    TranslationManager _translations;
    
    GameState _currentState{GameState::Menu};
    float _lastDt{0.0f};

    bool _isWaitingForKey{false};
    KeyAction _keyActionToRebind;
};

}  // namespace Client::Core