/**
 * File   : CreateRoomScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_CREATEROOMSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_CREATEROOMSCENE_HPP_

    #include "Interfaces/IScene.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "UI/UiFactory.hpp"
    #include "Utils/GameState.hpp"
    #include "Translation/TranslationManager.hpp"
    #include "Systems/NetworkSyncSystem.hpp"
    #include "Core/Settings.hpp"
    #include <SFML/Graphics.hpp>
    #include <memory>
    #include <functional>

namespace rtp::client {
    namespace scenes {
        /**
         * @class CreateRoomScene
         * @brief Scene for creating a new game room.
         * 
         * This scene allows players to set up a new game room by specifying
         * parameters such as room name, maximum players, and game settings.
         */
        class CreateRoomScene : public interfaces::IScene
        {
            public:
                /**
                 * @brief Constructor for CreateRoomScene
                 * @param registry Reference to the ECS registry
                 * @param settings Reference to the application settings
                 * @param network Reference to the client network
                 * @param window Reference to the SFML render window
                 */
                CreateRoomScene(ecs::Registry& UiRegistry,
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

                std::string _uiRoomName = "Room";           /**< Name of the room to be created */
                uint32_t _uiMaxPlayers = 4;                 /**< Maximum number of players in the room */
                float _uiDifficulty = 0.5f;                 /**< Difficulty setting for the room */
                float _uiSpeed = 1.0f;                      /**< Game speed setting for the room */
                uint32_t _uiDuration = 10;                  /**< Duration of the game in minutes */
                uint32_t _uiSeed = 42;                      /**< Seed for random generation */
                uint32_t _uiLevelId = 1;                    /**< Level ID for the game */
        };
    } // namespace scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_CREATEROOMSCENE_HPP_