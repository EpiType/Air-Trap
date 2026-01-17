/**
 * File   : LoginScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/scenes/LoginScene.hpp"

namespace rtp::client::scenes
{
    namespace
    {
        engine::ui::textContainer makeText(const std::string &content,
                                           const std::string &font,
                                           unsigned int size)
        {
            engine::ui::textContainer text;
            text.content = content;
            text.FontPath = font;
            text.fontSize = size;
            return text;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    LoginScene::LoginScene(engine::ecs::Registry& uiRegistry,
                           Settings& settings,
                           TranslationManager& translation,
                           NetworkSyncSystem& network,
                           std::function<void()> onLoggedIn)
        : _uiRegistry(uiRegistry),
          _settings(settings),
          _translation(translation),
          _network(network),
          _onLoggedIn(std::move(onLoggedIn))
    {
    }

    void LoginScene::onEnter(void)
    {
        const std::string title = _translation.get("login.title");
        const std::string usernameLabel = _translation.get("login.username");
        const std::string passwordLabel = _translation.get("login.password");
        const std::string loginLabel = _translation.get("login.action");
        const std::string registerLabel = _translation.get("register.action");

        engine::ui::UiFactory::createText(
            _uiRegistry,
            {520.0f, 80.0f},
            makeText(title == "login.title" ? "LOGIN" : title, "assets/fonts/title.ttf", 64),
            10,
            {255, 200, 100}
        );

        engine::ui::UiFactory::createText(
            _uiRegistry,
            {380.0f, 200.0f},
            makeText(usernameLabel == "login.username" ? "Username" : usernameLabel,
                     "assets/fonts/main.ttf", 24)
        );

        engine::ui::UiFactory::createTextInput(
            _uiRegistry,
            {360.0f, 235.0f},
            {560.0f, 45.0f},
            32,
            makeText("", "assets/fonts/main.ttf", 22),
            [this](const std::string& text) { _username = text; },
            [this](const std::string& text) { _username = text; }
        );

        engine::ui::UiFactory::createText(
            _uiRegistry,
            {380.0f, 310.0f},
            makeText(passwordLabel == "login.password" ? "Password" : passwordLabel,
                     "assets/fonts/main.ttf", 24)
        );

        engine::ui::UiFactory::createTextInput(
            _uiRegistry,
            {360.0f, 345.0f},
            {560.0f, 45.0f},
            32,
            makeText("", "assets/fonts/main.ttf", 22),
            [this](const std::string& text) { _password = text; },
            [this](const std::string& text) { _password = text; }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {360.0f, 430.0f},
            {270.0f, 60.0f},
            makeText(loginLabel == "login.action" ? "LOGIN" : loginLabel,
                     "assets/fonts/main.ttf", 22),
            [this]() { _network.tryLogin(_username, _password); }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {650.0f, 430.0f},
            {270.0f, 60.0f},
            makeText(registerLabel == "register.action" ? "REGISTER" : registerLabel,
                     "assets/fonts/main.ttf", 22),
            [this]() { _network.tryRegister(_username, _password); }
        );

        _loginNotified = false;
    }

    void LoginScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void LoginScene::handleEvent(const engine::input::Event &)
    {
    }

    void LoginScene::update(float)
    {
        (void)_settings;
        if (!_loginNotified && _network.isLoggedIn()) {
            _loginNotified = true;
            if (_onLoggedIn) {
                _onLoggedIn();
            }
        }
    }
} // namespace rtp::client::scenes
