/**
 * File   : LobbyScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/LobbyScene.hpp"

#include <cstddef>
#include <functional>
#include <list>
#include <string>

namespace rtp::client {
    namespace scenes {

        namespace {
            void hashCombine(std::size_t &seed, std::size_t value)
            {
                seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
            }

            std::size_t hashRoomList(const std::list<net::RoomInfo>& rooms)
            {
                std::size_t seed = 0;
                for (const auto& room : rooms) {
                    hashCombine(seed, std::hash<uint32_t>{}(room.roomId));
                    hashCombine(seed, std::hash<uint32_t>{}(room.currentPlayers));
                    hashCombine(seed, std::hash<uint32_t>{}(room.maxPlayers));
                    hashCombine(seed, std::hash<uint8_t>{}(room.inGame));
                    hashCombine(seed, std::hash<float>{}(room.difficulty));
                    hashCombine(seed, std::hash<float>{}(room.speed));
                    hashCombine(seed, std::hash<uint32_t>{}(room.duration));
                    hashCombine(seed, std::hash<uint32_t>{}(room.seed));
                    hashCombine(seed, std::hash<uint32_t>{}(room.levelId));
                    hashCombine(seed, std::hash<std::string>{}(std::string(room.roomName)));
                }
                return seed;
            }
        }

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
            log::info("Entering LobbyScene");

            _network.requestListRooms();
            _roomsHash = hashRoomList(_network.getAvailableRooms());
            buildUi();
            _uiBuilt = true;
        }

        void LobbyScene::buildUi(void)
        {
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
                    {900.0f, y},
                    {140.0f, 50.0f},
                    "JOIN",
                    [this, room]() {
                        _uiSelectedRoomId = room.roomId;
                        log::info("Attempting to join Room ID {}", room.roomId);
                        _network.tryJoinRoom(room.roomId, false);
                    }
                );

                _uiFactory.createButton(
                    _uiRegistry,
                    {1060.0f, y},
                    {120.0f, 50.0f},
                    "SPEC",
                    [this, room]() {
                        _uiSelectedRoomId = room.roomId;
                        log::info("Attempting to spectate Room ID {}", room.roomId);
                        _network.tryJoinRoom(room.roomId, true);
                    }
                );

                y += 60.0f;
                ++shown;
            }
        }

        void LobbyScene::onExit(void)
        {
            log::info("Exiting LobbyScene");
        }

        void LobbyScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void LobbyScene::update(float dt)
        {
            (void)dt;
            const auto rooms = _network.getAvailableRooms();
            const std::size_t newHash = hashRoomList(rooms);
            if (!_uiBuilt || newHash != _roomsHash) {
                _uiRegistry.clear();
                _roomsHash = newHash;
                buildUi();
                _uiBuilt = true;
            }
            if (_network.getState() == NetworkSyncSystem::State::InGame) {
                _changeState(GameState::Playing);
            } else if (_network.getState() == NetworkSyncSystem::State::InRoom) {
                _changeState(GameState::RoomWaiting);
            }
        }

    } // namespace scenes
} // namespace rtp::client
