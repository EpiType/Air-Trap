/**
 * File   : LoginScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/LoginScene.hpp"

namespace Client::Scenes {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    LoginScene::LoginScene(rtp::ecs::Registry& registry,
                         Client::Core::Settings& settings,
                         ClientNetwork& network,
                         sf::RenderWindow& window)
        : IScene(registry, settings, network, window) {}

    void LoginScene::onEnter()
    {
        // Initialize login scene resources here
    }

    void LoginScene::onExit()
    {
        // Clean up login scene resources here
    }

    void LoginScene::handleEvent(const sf::Event& event)
    {
        // Handle events specific to the login scene here
    }

    void LoginScene::update(float dt)
    {
        // Update login scene state here
        (void)dt;
    }

    void LoginScene::render()
    {
        // Render login scene here
    }

} // namespace Client::Scenes