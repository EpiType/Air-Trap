/**
 * File   : CreateRoomScene.cpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/scenes/CreateRoomScene.hpp"

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

    CreateRoomScene::CreateRoomScene(aer::ecs::Registry& uiRegistry,
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

    void CreateRoomScene::onEnter(void)
    {
        aer::ui::UiFactory::createText(
            _uiRegistry,
            {400.0f, 60.0f},
            makeText("CREATE ROOM", "assets/fonts/title.ttf", 52),
            10,
            {255, 200, 100}
        );

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {280.0f, 160.0f},
            makeText("Room Name:", "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createTextInput(
            _uiRegistry,
            {280.0f, 195.0f},
            {720.0f, 45.0f},
            32,
            makeText("Room name", "assets/fonts/main.ttf", 22),
            [this](const std::string &v) { _uiRoomName = v; },
            [this](const std::string &v) { _uiRoomName = v; }
        );

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {280.0f, 270.0f},
            makeText("Max Players :", "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createDropdown(
            _uiRegistry,
            {520.0f, 270.0f},
            {220.0f, 40.0f},
            {"2", "3", "4", "5", "6", "7", "8"},
            static_cast<int>(_uiMaxPlayers - 2),
            [this](int index) { _uiMaxPlayers = static_cast<uint32_t>(index + 2); }
        );

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {280.0f, 340.0f},
            makeText("Difficulty :", "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createSlider(
            _uiRegistry,
            {520.0f, 355.0f},
            {480.0f, 15.0f},
            0.1f,
            2.0f,
            _uiDifficulty,
            [this](float v) { _uiDifficulty = v; }
        );

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {280.0f, 410.0f},
            makeText("Speed :", "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createSlider(
            _uiRegistry,
            {520.0f, 425.0f},
            {480.0f, 15.0f},
            0.5f,
            3.0f,
            _uiSpeed,
            [this](float v) { _uiSpeed = v; }
        );

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {280.0f, 520.0f},
            {350.0f, 60.0f},
            makeText("CREATE", "assets/fonts/main.ttf", 22),
            [this]() {
                const std::string name = _uiRoomName.empty() ? "Room" : _uiRoomName;
                _networkSystem.tryCreateRoom(name, _uiMaxPlayers, _uiDifficulty,
                                             _uiSpeed, _uiDuration, _uiSeed,
                                             _uiLevelId);
            }
        );

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {650.0f, 520.0f},
            {350.0f, 60.0f},
            makeText("BACK", "assets/fonts/main.ttf", 22),
            [this]() { if (_changeScene) { _changeScene(SceneId::Lobby); } }
        );
    }

    void CreateRoomScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void CreateRoomScene::handleEvent(const aer::input::Event& event)
    {
        (void)event;
    }

    void CreateRoomScene::update(float dt)
    {
        (void)dt;
        if (_networkSystem.getState() == NetworkSyncSystem::State::InGame) {
            if (_changeScene) {
                _changeScene(SceneId::Playing);
            }
        } else if (_networkSystem.getState() == NetworkSyncSystem::State::InRoom) {
            if (_changeScene) {
                _changeScene(SceneId::RoomWaiting);
            }
        }
    }
} // namespace rtp::client::scenes
