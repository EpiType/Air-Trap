/**
 * File   : PlayingScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_PLAYINGSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_PLAYINGSCENE_HPP_

    #include "Interfaces/IScene.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "UI/UiFactory.hpp"
    #include "Utils/GameState.hpp"
    #include "Systems/NetworkSyncSystem.hpp"
    #include "Translation/TranslationManager.hpp"
    #include "Core/Settings.hpp"
    #include <SFML/Graphics.hpp>
    #include <memory>
    #include <functional>

namespace rtp::client {
    namespace Scenes {
        /**
         * @class PlayingScene
         * @brief Scene for the main gameplay.
         * 
         * This scene handles the core gameplay mechanics, rendering,
         * and user interactions during the game.
         */
        class PlayingScene : public interfaces::IScene
        {
            public:
                /**
                 * @brief Constructor for PlayingScene
                 * @param registry Reference to the ECS registry
                 * @param settings Reference to the application settings
                 * @param network Reference to the client network
                 * @param window Reference to the SFML render window
                 */
                PlayingScene(ecs::Registry& worldRegistry,
                                ecs::Registry& uiRegistry,
                                Settings& settings,
                                TranslationManager& translationManager,
                                NetworkSyncSystem& network,
                                graphics::UiFactory& uiFactory,
                                EntityBuilder& worldBuilder,
                                std::function<void(GameState)> changeState);

                /**
                 * @brief Type alias for a function that changes the game state.
                 */
                using ChangeStateFn = std::function<void(GameState)>;

                /**
                 * @brief Called when the scene is entered.
                 */
                void onEnter(void) override;

                /**
                 * @brief Called when the scene is exited.
                 */
                void onExit(void) override;

                /**
                 * @brief Handle an incoming event.
                 * @param event The event to handle.
                 */
                void handleEvent(const sf::Event& event) override;

                /**
                 * @brief Update the scene state.
                 * @param dt Time delta since the last update.
                 */
                void update(float dt) override;
            
            private:
                /**
                 * @brief Spawn parallax background entities.
                 */
                void spawnParallax(void);
                void sendChatMessage(void);
                void openChat(void);
                void closeChat(void);
                void updateChatHistoryText(void);

            private:
                ecs::Registry& _uiRegistry;                 /**< Reference to the ECS registry */
                Settings& _settings;                        /**< Reference to the application settings */
                TranslationManager& _translationManager;    /**< Reference to the translation manager */
                NetworkSyncSystem& _network;                /**< Reference to the client network */
                graphics::UiFactory& _uiFactory;            /**< UI Factory for creating UI components */
                ChangeStateFn _changeState;                 /**< Function to change the game state */
                ecs::Registry& _worldRegistry;              /**< Reference to the world ECS registry */
                EntityBuilder& _worldBuilder;               /**< Reference to the world entity builder */

                uint32_t _uiScore = 0;                      /**< Player score in the UI */
                uint32_t _uiFps = 0;                        /**< Current FPS in the UI */
                uint32_t _uiPing = 0;                       /**< Current ping in the UI */
                float _fpsTimer = 0.0f;                     /**< FPS timer accumulator */
                uint32_t _fpsFrames = 0;                    /**< FPS frame count */

                ecs::Entity _hudPing{};                /**< Entity for displaying ping in the HUD */
                ecs::Entity _hudFps{};                 /**< Entity for displaying FPS in the HUD */
                ecs::Entity _hudScore{};               /**< Entity for displaying score in the HUD */
                ecs::Entity _hudEntities{};            /**< Parent entity for HUD elements */
                ecs::Entity _hudAmmo{};                /**< Entity for displaying ammo */
                ecs::Entity _hudChargeBar{};           /**< Entity for charged shot HUD bar */
                bool _hudInit{false};                       /**< Flag indicating if HUD is initialized */

                bool _chatOpen{false};                      /**< Whether expanded chat is open */
                ecs::Entity _chatCompactPanel{};       /**< Compact chat background panel */
                ecs::Entity _chatCompactText{};        /**< Text entity for last chat message */

                ecs::Entity _chatPanel{};              /**< Expanded chat panel */
                ecs::Entity _chatHistoryText{};        /**< Text entity for chat history */
                ecs::Entity _chatInput{};              /**< Text input entity for chat */

                float _chargeTime{0.0f};                    /**< Local charge timer for HUD */
        };
    } // namespace Scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_PLAYINGSCENE_HPP_
