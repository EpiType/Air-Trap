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

                rtp::ecs::Entity _hudPing{};                /**< Entity for displaying ping in the HUD */
                rtp::ecs::Entity _hudFps{};                 /**< Entity for displaying FPS in the HUD */
                rtp::ecs::Entity _hudScore{};               /**< Entity for displaying score in the HUD */
                rtp::ecs::Entity _hudEntities{};            /**< Parent entity for HUD elements */
                bool _hudInit{false};                       /**< Flag indicating if HUD is initialized */
        };
    } // namespace Scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_PLAYINGSCENE_HPP_