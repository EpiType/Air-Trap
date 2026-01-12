/**
 * File   : PauseScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/PauseScene.hpp"

namespace rtp::client {
    namespace Scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        PauseScene::PauseScene(ecs::Registry& UiRegistry,
                               Settings& settings,
                               TranslationManager& translationManager,
                               NetworkSyncSystem& network,
                               graphics::UiFactory& uiFactory,
                               std::function<void(GameState)> changeState)
            : _uiRegistry(UiRegistry),
              _settings(settings),
              _translationManager(translationManager),
              _network(network),
              _uiFactory(uiFactory),
              _changeState(changeState)
        {
        }

        void PauseScene::onEnter(void)
        {
            rtp::log::info("Entering PauseScene");

            _uiFactory.createText(
                _uiRegistry,
                {500.0f, 200.0f},
                _translationManager.get("game.paused"),
                "assets/fonts/title.ttf",
                60,
                10,
                {255, 100, 100}
            );

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 350.0f},
                {300.0f, 60.0f},
                _translationManager.get("game.resume"),
                [this]() {
                    _changeState(GameState::Playing);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {540.0f, 400.0f},
                {200.0f, 60.0f},
                _translationManager.get("game.quit_to_menu"),
                [this]() {
                    _network.tryLeaveRoom();
                    _changeState(GameState::Menu);
                }
            );
        }

        void PauseScene::onExit(void)
        {
            rtp::log::info("Exiting PauseScene");
        }

        void PauseScene::handleEvent(const sf::Event& e)
        {
            if (const auto* kp = e.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    _changeState(GameState::Playing);
                }
            }
        }

        void PauseScene::update(float dt)
        {
            (void)dt;
        }
    } // namespace Scenes
} // namespace rtp::client