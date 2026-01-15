/**
 * File   : KeyBindingScene.hpp
 * License: MIT
 * Author : Elias Josué
 * HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   :
 * 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_KEYBINDINGSCENE_HPP_
#define RTYPE_CLIENT_SCENES_KEYBINDINGSCENE_HPP_

#include "Core/Settings.hpp"
#include "Interfaces/IScene.hpp"
#include "RType/ECS/Registry.hpp"
#include "Systems/NetworkSyncSystem.hpp"
#include "Translation/TranslationManager.hpp"
#include "UI/UiFactory.hpp"
#include "Utils/GameState.hpp"
#include "Utils/KeyAction.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace rtp::client
{
    /**
     * @class KeyBindingScene
     * @brief Scene for configuring key
     * bindings.
     */
    namespace scenes
    {
        /**
         * @class KeyBindingScene
         * @brief Scene for
         * configuring key bindings.
         * 
         * This scene allows
         * users to customize their key bindings for various
         * in-game
         * actions. It provides a user interface to select actions and
 * assign
         * new keys.
         */
        class KeyBindingScene : public interfaces::IScene {
            public:
                /**
                 * @brief Constructor for KeyBindingScene

                 * * @param registry Reference to the ECS registry
 * @param
                 * settings Reference to the application settings
 * @param
                 * network Reference to the client network
                 *
                 * @param window Reference to the SFML render window
 */
                KeyBindingScene(ecs::Registry &registry, Settings &settings,
                                TranslationManager &translationManager,
                                NetworkSyncSystem &network,
                                graphics::UiFactory &uiFactory,
                                std::function<void(GameState)> changeState);

                /**
                 * @brief Type alias for a function that
                 * changes the game state.
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

                 * * @param event The event to handle.
                 */
                void handleEvent(const sf::Event &event) override;

                /**
                 * @brief Update the scene state.
 * @param
                 * dt Time delta since the last update.
                 */
                void update(float dt) override;

            private:
                /**
                 * @brief Refresh the label of a button
                 * corresponding to a key action.
                 * @param
                 * action The key action whose button label needs to be
                 * refreshed.
                 */
                void refreshButtonLabel(KeyAction action);

            private:
                ecs::Registry &_uiRegistry; /**< Reference to the ECS registry */
                Settings &_settings; /**< Reference to the application settings */
                TranslationManager &_translationManager; /**< Reference to the translation
                                             manager */
                NetworkSyncSystem &_network; /**< Reference to the client network */
                graphics::UiFactory &_uiFactory; /**< UI Factory for creating UI components */
                ChangeStateFn _changeState; /**< Function to change the game state */

                bool _isWaitingForKey{false}; /**< Flag indicating if waiting for key input */
                KeyAction _actionToRebind{KeyAction::MoveUp}; /**< Action currently being rebound */

                std::unordered_map<KeyAction, ecs::Entity> _actionToButton; /**< Map of actions to their corresponding
                                        button entities */
        };
    } // namespace scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_KEYBINDINGSCENE_HPP_
