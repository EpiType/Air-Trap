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
#include "Game/EnemyBuilder.hpp"
#include "Translation/TranslationManager.hpp"
#include "Core/Settings.hpp"

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
    
    // ✅ Debug functions (dev only)
    void spawnEnemy();
    void killEnemy(std::size_t index);
    
    std::vector<rtp::ecs::Entity> _spawnedEnemy;
    sf::RenderWindow _window;
    rtp::ecs::Registry _registry;
    rtp::ecs::SystemManager _systemManager;
    Graphics::AssetManager _assetManager;
    Game::EnemyBuilder _enemyBuilder;
    Settings _settings;
    TranslationManager _translations;
    
    GameState _currentState{GameState::Menu};
    float _lastDt{0.0f};

    bool _isWaitingForKey{false};
    KeyAction _keyActionToRebind;
};

}  // namespace Client::Core