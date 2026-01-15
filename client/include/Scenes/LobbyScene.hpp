/**
 * File   : LobbyScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_LOBBYSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_LOBBYSCENE_HPP_

    #include "Interfaces/IScene.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "UI/UiFactory.hpp"
    #include "Utils/GameState.hpp"
    #include "Systems/NetworkSyncSystem.hpp"
    #include "Translation/TranslationManager.hpp"
    #include "Core/Settings.hpp"
    #include <SFML/Graphics.hpp>
    #include <memory>

namespace rtp::client {
    namespace scenes {
        /**
         * @class LobbyScene
         * @brief Scene for the game lobby.
         * 
         * This scene allows players to join existing game rooms or create new ones.
         * It displays a list of available rooms and provides options to manage them.
         */
        class LobbyScene : public interfaces::IScene
        {
            public:
                /**
                 * @brief Constructor for LobbyScene
                 * @param registry Reference to the ECS registry
                 * @param settings Reference to the application settings
                 * @param network Reference to the client network
                 * @param window Reference to the SFML render window
                 */
                LobbyScene(ecs::Registry& UiRegistry,
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

                uint32_t _uiSelectedRoomId = 0;             /**< Currently selected room ID in the UI */
                std::size_t _roomsHash = 0;                 /**< Hash of the last room list displayed */
                bool _uiBuilt = false;                      /**< Tracks if the UI has been built at least once */

                void buildUi(void);
        };
    } // namespace scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_LOBBYSCENE_HPP_
