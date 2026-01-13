/**
 * File   : MenuScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_MENUSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_MENUSCENE_HPP_

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
         * @class MenuScene
         * @brief Scene for the main menu.
         * 
         * This scene displays the main menu options such as Start Game,
         * Settings, and Exit. It handles user interactions to navigate
         * through the menu.
         */
        class MenuScene : public interfaces::IScene
        {
            public:
                /**
                 * @brief Constructor for MenuScene
                 * @param registry Reference to the ECS registry
                 * @param settings Reference to the application settings
                 * @param network Reference to the client network
                 * @param window Reference to the SFML render window
                 */
                MenuScene(ecs::Registry& UiRegistry,
                          Settings& settings,
                          TranslationManager& translationManager,
                          NetworkSyncSystem& network,
                          graphics::UiFactory& uiFactory,
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
                ecs::Registry& _uiRegistry;                 /**< Reference to the ECS registry */
                Settings& _settings;                        /**< Reference to the application settings */
                TranslationManager& _translationManager;    /**< Reference to the translation manager */
                NetworkSyncSystem& _network;                /**< Reference to the client network */
                graphics::UiFactory& _uiFactory;            /**< UI Factory for creating UI components */
                ChangeStateFn _changeState;                 /**< Function to change the game state */
        };
    } // namespace Scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_MENUSCENE_HPP_