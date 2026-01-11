/**
 * File   : LobbyScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/LobbyScene.hpp"

namespace rtp::client {
    namespace Scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        LobbyScene::LobbyScene(ecs::Registry& UiRegistry,
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

        void LobbyScene::onEnter(void)
        {
            rtp::log::info("Entering LobbyScene");

            _network.requestListRooms();
            float y = 230.0f;
            int shown = 0;

            _uiFactory.createText(
                _uiRegistry,
                {390.0f, 60.0f},
                "LOBBY - ROOMS",
                "assets/fonts/title.ttf",
                52,
                10,
                {255, 200, 100}
            );

            _uiFactory.createButton(
                _uiRegistry,
                {120.0f, 140.0f},
                {220.0f, 55.0f},
                "REFRESH",
                [this]() {
                    _network.requestListRooms();
                    _changeState(GameState::Lobby);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {360.0f, 140.0f},
                {260.0f, 55.0f},
                "CREATE ROOM",
                [this]() {
                    _changeState(GameState::CreateRoom);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {650.0f, 140.0f},
                {220.0f, 55.0f},
                "BACK",
                [this]() {
                    _changeState(GameState::Menu);
                }
            );

            for (const auto& room : _network.getAvailableRooms()) {
                if (shown >= 8) {
                    break;
                }

                std::string roomInfo = "Room " + std::to_string(room.roomId) +
                                       " | Players: " + std::to_string(room.currentPlayers) +
                                       "/" + std::to_string(room.maxPlayers) +
                                       " | Difficulty: " + std::to_string(room.difficulty);

                _uiFactory.createText(
                    _uiRegistry,
                    {140.0f, y + 10.0f},
                    roomInfo,
                    "assets/fonts/main.ttf",
                    22,
                    10
                );

                _uiFactory.createButton(
                    _uiRegistry,
                    {980.0f, y},
                    {160.0f, 50.0f},
                    "JOIN",
                    [this, room]() {
                        _uiSelectedRoomId = room.roomId;
                        _network.tryJoinRoom(room.roomId);
                    }
                );

                y += 60.0f;
                ++shown;
            }
        }

        void LobbyScene::onExit(void)
        {
            rtp::log::info("Exiting LobbyScene");
        }

        void LobbyScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void LobbyScene::update(float dt)
        {
            (void)dt;
        }

    } // namespace Scenes
} // namespace rtp::client