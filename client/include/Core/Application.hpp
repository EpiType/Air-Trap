/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Application
*/

#pragma once

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <memory>
#include <optional>
#include <vector>
#include <string>
#include <filesystem>
#include <unordered_map>

#include "Game/AssetManager.hpp"
#include "Game/EntityBuilder.hpp"
#include "Translation/TranslationManager.hpp"
#include "Network/ClientNetwork.hpp"
#include "Core/Settings.hpp"
#include "RType/Logger.hpp"

/* Components */
#include "RType/ECS/Components/Animation.hpp"
#include "RType/ECS/Components/Controllable.hpp"
#include "RType/ECS/Components/NetworkId.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Dropdown.hpp"
#include "RType/ECS/Components/UI/Slider.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/Velocity.hpp"

/* Utils */
#include "Utils/GameState.hpp"
#include "RType/Math/Vec2.hpp"

/* Systems */
#include "Systems/NetworkSyncSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/ParallaxSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/UISystem.hpp"
#include "Systems/UIRenderSystem.hpp"

/* ECS */
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/SystemManager.hpp"

/* Scenes */
#include "Interfaces/IScene.hpp"
#include "Scenes/CreateRoomScene.hpp"
#include "Scenes/KeyBindingScene.hpp"
#include "Scenes/LobbyScene.hpp"
#include "Scenes/LoginScene.hpp"
#include "Scenes/MenuScene.hpp"
#include "Scenes/PauseScene.hpp"
#include "Scenes/PlayingScene.hpp"
#include "Scenes/RoomWaitingScene.hpp"
#include "Scenes/SettingsScene.hpp"

namespace rtp::client {

namespace UIConstants {
    constexpr float WINDOW_WIDTH = 1280.0f;
    constexpr float WINDOW_HEIGHT = 720.0f;
}

class Application {
public:
    Application();
    void run(void); // to keep
    
private:
    void initUiECS(void); // to keep
    void initWorldECS(void); // to keep
    void initSystems(void); // to keep

    void setupSettingsCallbacks();

private:    
    void processInput(void); // to keep
    void update(sf::Time delta); // to keep
    void render(void); // to keep
    
private:
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
    std::unordered_map<GameState, std::unique_ptr<interfaces::IScene>> _scenes;
    interfaces::IScene* _activeScene = nullptr;

    Settings _settings;
    TranslationManager _translations;

    sf::RenderWindow _window;

    rtp::ecs::Registry _worldRegistry;
    rtp::ecs::Registry _uiRegistry;
    rtp::ecs::SystemManager _UiSystemManager;
    rtp::ecs::SystemManager _WorldSystemManager;

    AssetManager _assetManager;

    EntityBuilder _uiEntityBuilder;
    EntityBuilder _worldEntityBuilder;




    std::vector<rtp::ecs::Entity> _parallaxLayers;
    std::vector<rtp::ecs::Entity> _spawnedEnemy;
    std::vector<rtp::ecs::Entity> _projectiles;

    rtp::client::ClientNetwork _clientNetwork;
    
    sf::Shader _colorblindShader;
    sf::RenderTexture _renderTexture;
    bool _shaderLoaded{false};
    
    GameState _currentState{GameState::Login};
    float _lastDt{0.0f};

};

}  // namespace Client::Core