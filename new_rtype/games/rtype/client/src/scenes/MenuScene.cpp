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
                         NetworkSyncSystem& network,
                         std::function<void(SceneId)> changeScene)
        : _uiRegistry(uiRegistry),
          _settings(settings),
          _translation(translation),
          _networkSystem(network),
          _changeScene(changeScene)
    {
    }

    void MenuScene::onEnter(void)
    {
        engine::ui::UiFactory::createText(
            _uiRegistry,
            {400.0f, 100.0f},
            makeText(_translation.get("menu.title"), "assets/fonts/title.ttf", 72),
            10,
            {2, 100, 100}
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {490.0f, 300.0f},
            {300.0f, 60.0f},
            makeText(_translation.get("menu.play"), "assets/fonts/main.ttf", 24),
            [this]() {
                _networkSystem.requestListRooms();
                _changeScene(SceneId::Lobby);
            }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {490.0f, 380.0f},
            {300.0f, 60.0f},
            makeText(_translation.get("menu.settings"), "assets/fonts/main.ttf", 24),
            [this]() { if (_changeScene) { _changeScene(SceneId::Settings); } }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {490.0f, 460.0f},
            {300.0f, 60.0f},
            makeText(_translation.get("menu.exit"), "assets/fonts/main.ttf", 24),
            [this]() { exit(0); }
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
