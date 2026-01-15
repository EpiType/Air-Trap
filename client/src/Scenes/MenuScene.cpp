/**
 * File   : MenuScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/MenuScene.hpp"

namespace rtp::client {
    namespace scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        MenuScene::MenuScene(ecs::Registry& UiRegistry,
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

        void MenuScene::onEnter(void)
        {
            log::info("Entering MenuScene");

            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 100.0f},
                _translationManager.get("menu.title"),
                "assets/fonts/title.ttf",
                72,
                10,
                {2, 100, 100}
            );

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 300.0f},
                {300.0f, 60.0f},
                _translationManager.get("menu.play"),
                [this]() {
                    _network.requestListRooms();
                    _changeState(GameState::Lobby);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 380.0f},
                {300.0f, 60.0f},
                _translationManager.get("menu.settings"),
                [this]() {
                    _changeState(GameState::Settings);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 460.0f},
                {300.0f, 60.0f},
                _translationManager.get("menu.mods"),
                [this]() {
                    _changeState(GameState::ModMenu);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 540.0f},
                {300.0f, 60.0f},
                _translationManager.get("menu.exit"),
                [this]() {
                    std::exit(0);
                }
            );
        }

        void MenuScene::onExit(void)
        {
            log::info("Exiting MenuScene");
        }

        void MenuScene::handleEvent(const sf::Event& e)
        {
            if (const auto* kp = e.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    std::exit(0);
                }
            }
        }

        void MenuScene::update(float dt)
        {
            (void)dt;
        }

    } // namespace scenes
} // namespace rtp::client