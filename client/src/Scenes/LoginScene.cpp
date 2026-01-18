/**
 * File   : LoginScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/LoginScene.hpp"

namespace rtp::client {
    namespace scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        LoginScene::LoginScene(ecs::Registry& UiRegistry,
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

        void LoginScene::onEnter(void)
        {
            log::info("Entering LoginScene");

            _uiFactory.createText(
                _uiRegistry,
                {520.0f, 80.0f},
                "LOGIN",
                "assets/fonts/title.ttf",
                64,
                10,
                {255, 200, 100}
            );

            _uiFactory.createText(
                _uiRegistry,
                {380.0f, 200.0f},
                "Username :",
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createTextInput(
                _uiRegistry,
                {360.0f, 235.0f},
                {560.0f, 45.0f},
                "assets/fonts/main.ttf",
                22,
                32,
                "",
                [this](const std::string& text) { _uiUsername = text; },
                [this](const std::string& text) { _uiUsername = text; }
            );

            _uiFactory.createText(
                _uiRegistry,
                {360.0f, 310.0f},
                "Password :",
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createTextInput(
                _uiRegistry,
                {360.0f, 345.0f},
                {560.0f, 45.0f},
                "assets/fonts/main.ttf",
                22,
                32,
                "",
                [this](const std::string& text) { _uiPassword = text; },
                [this](const std::string& text) { _uiPassword = text; }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {360.0f, 430.0f},
                {270.0f, 60.0f},
                "LOGIN",
                [this]() {
                    auto weaponKind = static_cast<uint8_t>(_settings.getSelectedWeapon());
                    _network.tryLogin(_uiUsername, _uiPassword, weaponKind);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {650.0f, 430.0f},
                {270.0f, 60.0f},
                "REGISTER",
                [this]() { _network.tryRegister(_uiUsername, _uiPassword); }
            );
        }

        void LoginScene::onExit(void)
        {
            log::info("Exiting LoginScene");
        }

        void LoginScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void LoginScene::update(float dt)
        {
            (void)dt;
            if (_network.isLoggedIn()) {
                _changeState(GameState::Menu);
            }
        }

    } // namespace scenes
} // namespace rtp::client
