/**
 * File   : IScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_INTERFACES_ISCENE_HPP_
    #define RTYPE_CLIENT_INTERFACES_ISCENE_HPP_

    #include <SFML/Graphics.hpp>

namespace rtp::client {
    namespace interfaces {

        /**
         * @interface IScene
         * @brief Interface for different scenes in the client application.
         * 
         * This interface defines the basic structure and functionalities that
         * all scenes (e.g., Menu, Game, Settings) must implement.
         */
        class IScene {
            public:
                virtual ~IScene(void) = default;

                /**
                 * @brief Called when the scene is entered.
                 * @note Used for initializing scene-specific resources.
                 */
                virtual void onEnter(void) = 0;

                /**
                 * @brief Called when the scene is exited.
                 * @note Used for cleaning up scene-specific resources.
                 */
                virtual void onExit(void) = 0;

                /**
                 * @brief Handle an incoming event.
                 * @param event The event to handle.
                 */
                virtual void handleEvent(const sf::Event&) = 0;

                /**
                 * @brief Update the scene state.
                 * @param dt Time delta since the last update.
                 */
                virtual void update(float dt) = 0;
        };
    } // namespace interfaces
} // namespace rtp::client

#endif // RTYPE_CLIENT_INTERFACES_ISCENE_HPP_