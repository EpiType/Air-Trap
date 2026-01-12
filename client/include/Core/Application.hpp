/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Application
*/

#ifndef RTYPE_CLIENT_CORE_APPLICATION_HPP_
    #define RTYPE_CLIENT_CORE_APPLICATION_HPP_

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
#include "Utils/KeyAction.hpp"
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
            void run(void);
            
        private:
            void initUiECS(void);
            void initWorldECS(void);
            void initUiSystems(void);
            void initWorldSystems(void);

            void setupSettingsCallbacks();

        private:
            void processInput(void);
            void update(sf::Time delta);
            void render(void);
            
            void changeState(GameState newState);

        private:
            std::unordered_map<GameState, std::unique_ptr<interfaces::IScene>> _scenes;
            interfaces::IScene* _activeScene = nullptr;

            Settings _settings;
            TranslationManager _translations;

            sf::RenderWindow _window;

            rtp::ecs::Registry _worldRegistry;
            rtp::ecs::Registry _uiRegistry;
            rtp::ecs::SystemManager _uiSystemManager;
            rtp::ecs::SystemManager _worldSystemManager;

            AssetManager _assetManager;

            EntityBuilder _uiEntityBuilder;
            EntityBuilder _worldEntityBuilder;

            rtp::client::ClientNetwork _clientNetwork;
            
            sf::Shader _colorblindShader;
            sf::RenderTexture _renderTexture;
            bool _shaderLoaded{false};
            
            GameState _currentState{GameState::Login};
            float _lastDt{0.0f};
    };
}  // namespace Client::Core

#endif /* !RTYPE_CLIENT_CORE_APPLICATION_HPP_ */