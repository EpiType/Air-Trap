/**
 * File   : MenuScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/scenes/MenuScene.hpp"

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

    MenuScene::MenuScene(engine::ecs::Registry& uiRegistry,
                         Settings& settings,
                         TranslationManager& translation,
                         std::function<void()> onPlay,
                         std::function<void()> onSettings,
                         std::function<void()> onLogout)
        : _uiRegistry(uiRegistry),
          _settings(settings),
          _translation(translation),
          _onPlay(std::move(onPlay)),
          _onSettings(std::move(onSettings)),
          _onLogout(std::move(onLogout))
    {
    }

    void MenuScene::onEnter(void)
    {
        const std::string title = _translation.get("menu.title");
        const std::string playLabel = _translation.get("menu.play");
        const std::string settingsLabel = _translation.get("menu.settings");
        const std::string logoutLabel = _translation.get("menu.logout");

        engine::ui::UiFactory::createText(
            _uiRegistry,
            {520.0f, 80.0f},
            makeText(title == "menu.title" ? "MENU" : title, "assets/fonts/title.ttf", 64),
            10,
            {255, 200, 100}
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {420.0f, 240.0f},
            {440.0f, 70.0f},
            makeText(playLabel == "menu.play" ? "PLAY" : playLabel,
                     "assets/fonts/main.ttf", 24),
            [this]() { if (_onPlay) { _onPlay(); } }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {420.0f, 330.0f},
            {440.0f, 70.0f},
            makeText(settingsLabel == "menu.settings" ? "SETTINGS" : settingsLabel,
                     "assets/fonts/main.ttf", 24),
            [this]() { if (_onSettings) { _onSettings(); } }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {420.0f, 420.0f},
            {440.0f, 70.0f},
            makeText(logoutLabel == "menu.logout" ? "LOGOUT" : logoutLabel,
                     "assets/fonts/main.ttf", 24),
            [this]() { if (_onLogout) { _onLogout(); } }
        );
    }

    void MenuScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void MenuScene::handleEvent(const engine::input::Event &)
    {
    }

    void MenuScene::update(float)
    {
        (void)_settings;
    }
} // namespace rtp::client::scenes
