/**
 * File   : RegisterScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/scenes/RegisterScene.hpp"

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

    RegisterScene::RegisterScene(engine::ecs::Registry& uiRegistry,
                                 Settings& settings,
                                 TranslationManager& translation,
                                 NetworkSyncSystem& network,
                                 std::function<void()> onRegistered,
                                 std::function<void()> onBack)
        : _uiRegistry(uiRegistry),
          _settings(settings),
          _translation(translation),
          _network(network),
          _onRegistered(std::move(onRegistered)),
          _onBack(std::move(onBack))
    {
    }

    void RegisterScene::onEnter(void)
    {
        const std::string title = _translation.get("register.title");
        const std::string usernameLabel = _translation.get("register.username");
        const std::string passwordLabel = _translation.get("register.password");
        const std::string registerLabel = _translation.get("register.action");
        const std::string backLabel = _translation.get("common.back");

        engine::ui::UiFactory::createText(
            _uiRegistry,
            {480.0f, 80.0f},
            makeText(title == "register.title" ? "REGISTER" : title, "assets/fonts/title.ttf", 64),
            10,
            {255, 200, 100}
        );

        engine::ui::UiFactory::createText(
            _uiRegistry,
            {380.0f, 200.0f},
            makeText(usernameLabel == "register.username" ? "Username" : usernameLabel,
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
            makeText(passwordLabel == "register.password" ? "Password" : passwordLabel,
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
            makeText(registerLabel == "register.action" ? "REGISTER" : registerLabel,
                     "assets/fonts/main.ttf", 22),
            [this]() { _network.tryRegister(_username, _password); }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {650.0f, 430.0f},
            {270.0f, 60.0f},
            makeText(backLabel == "common.back" ? "BACK" : backLabel,
                     "assets/fonts/main.ttf", 22),
            [this]() {
                if (_onBack) {
                    _onBack();
                }
            }
        );

        _registeredNotified = false;
    }

    void RegisterScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void RegisterScene::handleEvent(const engine::input::Event &)
    {
    }

    void RegisterScene::update(float)
    {
        (void)_settings;
        if (!_registeredNotified && _network.isLoggedIn()) {
            _registeredNotified = true;
            if (_onRegistered) {
                _onRegistered();
            }
        }
    }
} // namespace rtp::client::scenes
