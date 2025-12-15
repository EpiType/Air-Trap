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
#include "Systems/RenderSystem.hpp"
#include "Systems/TranslationManager.hpp"
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
    void setupSettingsCallbacks();  // ✅ Nouveau
    
    void processInput();
    void update(sf::Time delta);
    void render();
    
    void changeState(GameState newState);

    sf::RenderWindow _window;
    rtp::ecs::Registry _registry;
    rtp::ecs::SystemManager _systemManager;
    Graphics::AssetManager _assetManager;
    Settings _settings;
    TranslationManager _translations;
    
    GameState _currentState{GameState::Menu};
    float _lastDt{0.0f};
};

}  // namespace Client::Core