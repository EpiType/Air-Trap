/**
 * File   : LobbyScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/scenes/LobbyScene.hpp"

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

    LobbyScene::LobbyScene(engine::ecs::Registry& uiRegistry,
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

    void LobbyScene::onEnter(void)
    {
        const std::string title = _translation.get("lobby.title");
        const std::string createLabel = _translation.get("lobby.create");
        const std::string refreshLabel = _translation.get("lobby.refresh");
        const std::string backLabel = _translation.get("common.back");

        engine::ui::UiFactory::createText(
            _uiRegistry,
            {520.0f, 80.0f},
            makeText(title == "lobby.title" ? "LOBBY" : title, "assets/fonts/title.ttf", 64),
            10,
            {255, 200, 100}
        );

        engine::ui::UiFactory::createText(
            _uiRegistry,
            {360.0f, 210.0f},
            makeText("Room name", "assets/fonts/main.ttf", 22)
        );

        engine::ui::UiFactory::createTextInput(
            _uiRegistry,
            {360.0f, 245.0f},
            {560.0f, 45.0f},
            32,
            makeText("", "assets/fonts/main.ttf", 22),
            [this](const std::string &text) { _roomName = text; },
            [this](const std::string &text) { _roomName = text; }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {360.0f, 320.0f},
            {270.0f, 60.0f},
            makeText(createLabel == "lobby.create" ? "CREATE" : createLabel,
                     "assets/fonts/main.ttf", 22),
            [this]() {
                _networkSystem.tryCreateRoom(_roomName.empty() ? "Room" : _roomName,
                                       4,
                                       1.0f,
                                       1.0f,
                                       0,
                                       0,
                                       0);
            }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {650.0f, 320.0f},
            {270.0f, 60.0f},
            makeText(refreshLabel == "lobby.refresh" ? "REFRESH" : refreshLabel,
                     "assets/fonts/main.ttf", 22),
            [this]() { _networkSystem.requestListRooms(); }
        );

        engine::ui::UiFactory::createButton(
            _uiRegistry,
            {360.0f, 410.0f},
            {560.0f, 60.0f},
            makeText(backLabel == "common.back" ? "BACK" : backLabel,
                     "assets/fonts/main.ttf", 22),
            [this]() { if (_changeScene) { _changeScene(SceneId::Menu); } }
        );
    }

    void LobbyScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void LobbyScene::handleEvent(const engine::input::Event &)
    {
    }

    void LobbyScene::update(float)
    {
        (void)_settings;
    }
} // namespace rtp::client::scenes
