/**
 * File   : PauseScene.cpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/scenes/PauseScene.hpp"

namespace rtp::client::scenes
{
    namespace
    {
        aer::ui::textContainer makeText(const std::string &content,
                                        const std::string &font,
                                        unsigned int size)
        {
            aer::ui::textContainer text;
            text.content = content;
            text.FontPath = font;
            text.fontSize = size;
            return text;
        }
    }

    PauseScene::PauseScene(aer::ecs::Registry& uiRegistry,
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

    void PauseScene::onEnter(void)
    {
        aer::ui::UiFactory::createText(
            _uiRegistry,
            {500.0f, 200.0f},
            makeText(_translation.get("game.paused"), "assets/fonts/title.ttf", 60),
            10,
            {255, 100, 100}
        );

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {490.0f, 350.0f},
            {300.0f, 60.0f},
            makeText(_translation.get("game.resume"), "assets/fonts/main.ttf", 22),
            [this]() {
                if (_changeScene) {
                    _changeScene(SceneId::Playing);
                }
            }
        );

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {540.0f, 400.0f},
            {200.0f, 60.0f},
            makeText(_translation.get("game.quit_to_menu"), "assets/fonts/main.ttf", 20),
            [this]() {
                _networkSystem.tryLeaveRoom();
                if (_changeScene) {
                    _changeScene(SceneId::Menu);
                }
            }
        );
    }

    void PauseScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void PauseScene::handleEvent(const aer::input::Event &event)
    {
        if (event.type == aer::input::EventType::KeyDown &&
            event.key == aer::input::KeyCode::Escape) {
            if (_changeScene) {
                _changeScene(SceneId::Playing);
            }
        }
    }

    void PauseScene::update(float dt)
    {
        (void)dt;
    }
} // namespace rtp::client::scenes
