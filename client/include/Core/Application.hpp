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
#include "UI/UiFactory.hpp"

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
#include "RType/ECS/Components/UI/SpritePreview.hpp"
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
#include "Scenes/GamepadSettingsScene.hpp"
#include "Scenes/KeyBindingScene.hpp"
#include "Scenes/LobbyScene.hpp"
#include "Scenes/LoginScene.hpp"
#include "Scenes/MenuScene.hpp"
#include "Scenes/ModMenuScene.hpp"
#include "Scenes/PauseScene.hpp"
#include "Scenes/PlayingScene.hpp"
#include "Scenes/RoomWaitingScene.hpp"
#include "Scenes/SettingsScene.hpp"

/**
 * @namespace rtp::client
 * @brief R-Type client namespace
 */
namespace rtp::client {

    /**
     * @namespace UIConstants
     * @brief Constants related to the UI
     */
    namespace UIConstants {
        constexpr float WINDOW_WIDTH = 1280.0f;     /**< Standard window width */
        constexpr float WINDOW_HEIGHT = 720.0f;     /**< Standard window height */
    }

    /**
     * @class Application
     * @brief Main application class for the R-Type client.
     * 
     * This class manages the main application loop, including initialization
     * of systems, scenes, and handling of game states.
     */
    class Application {
        public:
            /**
             * @brief Construct a new Application object
             */
            Application(const std::string& serverIp, uint16_t serverPort);

            /**
             * @brief Run the main application loop
             */
            void run(void);
            
        private:
            /**
             * @brief Initialize the ECS and systems for the UI
             */
            void initUiECS(void);

            /**
             * @brief Initialize the ECS and systems for the game world
             */
            void initWorldECS(void);

            /**
             * @brief Initialize the game world systems
             */
            void initUiSystems(void);

            /**
             * @brief Initialize the UI systems
             */
            void initWorldSystems(void);

            /**
             * @brief Initialize all application scenes
             */
            void initScenes(void);

            /**
             * @brief Setup callbacks for settings changes
             */
            void setupSettingsCallbacks();

        private:
            /**
             * @brief Handle input events
             */
            void processInput(void);

            /**
             * @brief Update the application state
             * @param delta Time delta since the last update
             */
            void update(sf::Time delta);

            /**
             * @brief Render the current scene
             */
            void render(void);
            
            /**
             * @brief Change the current game state and active scene
             * @param newState The new game state to switch to
             */
            void changeState(GameState newState);

        private:
            std::unordered_map<GameState,
                std::unique_ptr<interfaces::IScene>> _scenes;   /**< Map of game states to their corresponding scenes */
            interfaces::IScene* _activeScene = nullptr;         /**< Pointer to the currently active scene */

            graphics::UiFactory _uiFactory;                     /**< UI Factory for creating UI components */

            Settings _settings;                                 /**< Application settings manager */
            TranslationManager _translations;                   /**< Translation manager for localization */

            sf::RenderWindow _window;                           /**< SFML render window */

            rtp::ecs::Registry _worldRegistry;                  /**< ECS registry for the game world */
            rtp::ecs::Registry _uiRegistry;                     /**< ECS registry for the UI */
            rtp::ecs::SystemManager _uiSystemManager;           /**< ECS system manager for the UI */
            rtp::ecs::SystemManager _worldSystemManager;        /**< ECS system manager for the game world */

            AssetManager _assetManager;                         /**< Asset manager for textures and fonts */

            EntityBuilder _uiEntityBuilder;                     /**< Entity builder for UI entities */
            EntityBuilder _worldEntityBuilder;                  /**< Entity builder for world entities */

            rtp::client::ClientNetwork _clientNetwork;          /**< Client network manager */
            
            sf::Shader _colorblindShader;                       /**< Shader for colorblind mode */
            sf::RenderTexture _renderTexture;                   /**< Render texture for off-screen rendering */
            bool _shaderLoaded{false};                          /**< Flag indicating if the shader was loaded successfully */
            
            GameState _currentState{GameState::NotInit};        /**< Current game state */
            float _lastDt{0.0f};                                /**< Last delta time value */
    };
}  // namespace Client::Core

#endif /* !RTYPE_CLIENT_CORE_APPLICATION_HPP_ */