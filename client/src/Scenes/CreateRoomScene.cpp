/**
 * File   : CreateRoomScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/CreateRoomScene.hpp"

namespace rtp::client {
    namespace Scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        CreateRoomScene::CreateRoomScene(ecs::Registry& UiRegistry,
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

        void CreateRoomScene::onEnter()
        {
            rtp::log::info("CreateRoomScene entered");

            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 60.0f},
                "CREATE ROOM",
                "assets/fonts/title.ttf",
                52,
                10,
                {255, 200, 100}
            );

            _uiFactory.createText(
                _uiRegistry,
                {280.0f, 160.0f},
                "Room Name:",
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createTextInput(
                _uiRegistry,
                {280.0f, 195.0f},
                {720.0f, 45.0f},
                "assets/fonts/main.ttf",
                22,
                32,
                "Room name",
                [this](const std::string &v) { _uiRoomName = v; },
                [this](const std::string &v) { _uiRoomName = v; }
            );

            _uiFactory.createText(
                _uiRegistry,
                {280.0f, 270.0f},
                "Max Players :",
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createDropdown(
                _uiRegistry,
                {520.0f, 270.0f},
                {220.0f, 40.0f},
                {"2", "3", "4", "5", "6", "7", "8"},
                _uiMaxPlayers - 2,
                [this](int index) { _uiMaxPlayers = index + 2; }
            );

            _uiFactory.createText(
                _uiRegistry,
                {280.0f, 340.0f},
                "Difficulty :",
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createSlider(
                _uiRegistry,
                {520.0f, 355.0f},
                {480.0f, 15.0f},
                0.1f,
                2.0f,
                1.0f,
                [this](float v) { _uiDifficulty = v; }
            );

            _uiFactory.createText(
                _uiRegistry,
                {280.0f, 410.0f},
                "Speed :",
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createSlider(
                _uiRegistry,
                {520.0f, 425.0f},
                {480.0f, 15.0f},
                0.5f,
                3.0f,
                1.0f,
                [this](float v) { _uiSpeed = v; }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {280.0f, 520.0f},
                {350.0f, 60.0f},
                "CREATE",
                [this]() {
                    const std::string name =
                        _uiRoomName.empty() ? "Room" : _uiRoomName;

                    _network.tryCreateRoom(name, _uiMaxPlayers, _uiDifficulty,
                                      _uiSpeed, _uiDuration, _uiSeed,
                                      _uiLevelId);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {650.0f, 520.0f},
                {350.0f, 60.0f},
                "BACK",
                [this]() { _changeState(GameState::Lobby); }
            );
        };

        void CreateRoomScene::onExit()
        {
            rtp::log::info("CreateRoomScene exited");
        }

        void CreateRoomScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void CreateRoomScene::update(float dt)
        {
            (void)dt;
            if (_network.getState() == NetworkSyncSystem::State::InGame) {
                _changeState(GameState::Playing);
            } else if (_network.getState() == NetworkSyncSystem::State::InRoom) {
                _changeState(GameState::RoomWaiting);
            }
        }

        //////////////////////////////////////////////////////////////////////////
        // Private API
        //////////////////////////////////////////////////////////////////////////

    } // namespace Scenes
} // namespace rtp::client
