/**
 * File   : RoomWaitingScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_ROOMWAITINGSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_ROOMWAITINGSCENE_HPP_

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
         * @class RoomWaitingScene
         * @brief Scene for waiting in a game room.
         * 
         * This scene displays the current room status, including the list of
         * players in the room and options to ready up or leave the room.
         */
        class RoomWaitingScene : public interfaces::IScene
        {
            public:
                /**
                 * @brief Constructor for RoomWaitingScene
                 * @param registry Reference to the ECS registry
                 * @param settings Reference to the application settings
                 * @param network Reference to the client network
                 * @param window Reference to the SFML render window
                 */
                RoomWaitingScene(ecs::Registry& UiRegistry,
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

                bool _uiReady = false;                      /**< Player ready status in the UI */
                bool _chatOpen = false;                     /**< Whether expanded chat is open */

                ecs::Entity _chatCompactPanel{};       /**< Compact chat background panel */
                ecs::Entity _chatCompactText{};        /**< Text entity for last chat message */
                ecs::Entity _chatToggleButton{};       /**< Toggle button for chat */

                ecs::Entity _chatPanel{};              /**< Expanded chat panel */
                ecs::Entity _chatHistoryText{};        /**< Text entity for chat history */
                ecs::Entity _chatInput{};              /**< Text input entity for chat */

                void openChat(void);
                void closeChat(void);
                void updateChatHistoryText(void);
                void sendChatMessage(void);
        };
    } // namespace Scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_ROOMWAITINGSCENE_HPP_
