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

namespace rtp::client {

namespace UIConstants {
    constexpr float WINDOW_WIDTH = 1280.0f;
    constexpr float WINDOW_HEIGHT = 720.0f;
    
    constexpr float TITLE_X = 400.0f;
    constexpr float TITLE_Y = 100.0f;
    constexpr float TITLE_FONT_SIZE = 72.0f;
    
    constexpr float BUTTON_WIDTH = 300.0f;
    constexpr float BUTTON_HEIGHT = 60.0f;
    constexpr float BUTTON_X = 490.0f;
    constexpr float BUTTON_SPACING = 80.0f;
    constexpr float BUTTON_START_Y = 300.0f;
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
    void run(); // to keep
    
private:
    void initECS(); // to keep
    // void initMenu();
    // void initGame();
    // void initSettingsMenu();
    // void initKeyBindingMenu();
    // void initLoginScene();
    // void initLobbyScene();
    // void initCreateRoomScene();
    // void initRoomWaitingScene();
    // void initPauseMenu();
    void setupSettingsCallbacks();

private:    
    void processInput(); // to keep
    void update(sf::Time delta); // to keep
    void render(); // to keep
    
private:
    void processMenuInput(const sf::Event& event);
    void processGameInput(const sf::Event& event);
    void processSettingsInput(const sf::Event& event);
    void processKeyBindingInput(const sf::Event& event);
    void handleGlobalEscape(); // to keep
    
    void changeState(GameState newState); // to keep
    
    void createParallaxBackground();
    void spawnEnemy(const rtp::Vec2f& position);
    void spawnEnemy2(const rtp::Vec2f& position);
    void spawnProjectile(const rtp::Vec2f& position);
    void killEnemy(std::size_t index);
    void killProjectile(std::size_t index);

    void clearRoomWaitingUiEntities();

private:
    std::vector<rtp::ecs::Entity> _parallaxLayers;
    std::vector<rtp::ecs::Entity> _spawnedEnemy;
    std::vector<rtp::ecs::Entity> _projectiles;
    sf::RenderWindow _window;
    rtp::ecs::Registry _worldRegistry;
    rtp::ecs::Registry _uiRegistry;
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

    // bool _uiReady = false;

    // rtp::ecs::Entity _hudScore{};
    // rtp::ecs::Entity _hudPing{};
    // rtp::ecs::Entity _hudFps{};
    // rtp::ecs::Entity _hudEntities{};
    // bool _hudInit = false;

    // int _score = 12345;

};

}  // namespace Client::Core