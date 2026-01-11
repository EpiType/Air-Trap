/**
 * File   : KeyBindingScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_KEYBINDINGSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_KEYBINDINGSCENE_HPP_

    #include "Interfaces/IScene.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "UI/UiFactory.hpp"
    #include "Network/ClientNetwork.hpp"
    #include "Core/Settings.hpp"
    #include <SFML/Graphics.hpp>
    #include <memory>
    #include <functional>
    #include <string>
    #include <cstdint>
    #include <vector>

enum class GameState;
enum class KeyAction;

namespace rtp::client {

    /**
     * @enum KeyAction
     * @brief Actions that can be bound to keys
     */
    enum class KeyAction {
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        Shoot,
        Pause,
        Menu
    };

    /**
     * @class KeyBindingScene
     * @brief Scene for configuring key bindings.
     */
    namespace Scenes {
        /**
         * @class KeyBindingScene
         * @brief Scene for configuring key bindings.
         * 
         * This scene allows users to customize their key bindings for various
         * in-game actions. It provides a user interface to select actions and
         * assign new keys.
         */
        class KeyBindingScene : public interfaces::IScene
        {
            public:
                /**
                 * @brief Constructor for KeyBindingScene
                 * @param registry Reference to the ECS registry
                 * @param settings Reference to the application settings
                 * @param network Reference to the client network
                 * @param window Reference to the SFML render window
                 */
                KeyBindingScene(rtp::ecs::Registry& registry,
                                Settings& settings,
                                ClientNetwork& network,
                                graphics::UiFactory& uiFactory,
                                std::function<void(GameState)> changeState);

                /**
                 * @brief Type alias for a function that changes the game state.
                 */
                using ChangeStateFn = std::function<void(GameState)>;

                /**
                 * @brief Called when the scene is entered.
                 */
                void onEnter() override;

                /**
                 * @brief Called when the scene is exited.
                 */
                void onExit() override;

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
                 * @brief Refresh the label of a button corresponding to a key action.
                 * @param action The key action whose button label needs to be refreshed.
                 */
                void refreshButtonLabel(KeyAction action);
            
            private:
                ecs::Registry& _uiRegistry;                 /**< Reference to the ECS registry */
                Settings& _settings;                        /**< Reference to the application settings */
                ClientNetwork& _network;                    /**< Reference to the client network */
                graphics::UiFactory& _uiFactory;            /**< UI Factory for creating UI components */
                ChangeStateFn _changeState;                 /**< Function to change the game state */

                bool _isWaitingForKey{false};               /**< Flag indicating if waiting for key input */
                KeyAction _actionToRebind;                  /**< Action currently being rebound */

                std::unordered_map<KeyAction,
                    rtp::ecs::Entity> _actionToButton;      /**< Map of actions to their corresponding button entities */
        };
    } // namespace Scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_KEYBINDINGSCENE_HPP_