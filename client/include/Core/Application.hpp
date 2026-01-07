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
#include "Network/ClientNetwork.hpp"
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
    Login,
    Lobby,
    CreateRoom,
    RoomWaiting,
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
    void initLoginScene();
    void initLobbyScene();
    void initCreateRoomScene();
    void initRoomWaitingScene();
    void initPauseMenu();
    void setupSettingsCallbacks();

private:    
    void processInput();
    void update(sf::Time delta);
    void render();
    
private:
    void processMenuInput(const sf::Event& event);
    void processGameInput(const sf::Event& event);
    void processSettingsInput(const sf::Event& event);
    void processKeyBindingInput(const sf::Event& event);
    void handleGlobalEscape();
    
    void changeState(GameState newState);
    
    void createParallaxBackground();
    void spawnEnemy(const rtp::Vec2f& position);
    void spawnEnemy2(const rtp::Vec2f& position);
    void spawnProjectile(const rtp::Vec2f& position);
    void killEnemy(std::size_t index);
    void killProjectile(std::size_t index);

private:
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

    rtp::client::ClientNetwork _clientNetwork;
    
    sf::Shader _colorblindShader;
    sf::RenderTexture _renderTexture;
    bool _shaderLoaded{false};
    
    GameState _currentState{GameState::Login};
    float _lastDt{0.0f};

    bool _isWaitingForKey{false};
    KeyAction _keyActionToRebind;

    std::string _uiUsername;
    std::string _uiPassword;

    uint32_t _uiSelectedRoomId = 0;

    std::string _uiRoomName = "Room";
    uint32_t _uiMaxPlayers = 4;
    float _uiDifficulty = 0.5f;
    float _uiSpeed = 1.0f;
    uint32_t _uiDuration = 10;
    uint32_t _uiSeed = 42;
    uint32_t _uiLevelId = 1;

    bool _uiReady = false;
};

}  // namespace Client::Core