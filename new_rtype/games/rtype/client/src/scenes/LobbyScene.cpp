/**
 * File   : LobbyScene.cpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/scenes/LobbyScene.hpp"

#include <functional>

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

        void hashCombine(std::size_t &seed, std::size_t value)
        {
            seed ^= value + 0x9e3779b97f4a7c15ULL + (seed << 6) + (seed >> 2);
        }

        std::size_t hashRoomList(const std::list<rtp::net::RoomInfo> &rooms)
        {
            std::size_t seed = 0;
            for (const auto &room : rooms) {
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

    LobbyScene::LobbyScene(aer::ecs::Registry& uiRegistry,
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
        _networkSystem.requestListRooms();
        _roomsHash = hashRoomList(_networkSystem.getAvailableRooms());
        buildUi();
        _uiBuilt = true;
    }

    void LobbyScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
        _uiBuilt = false;
    }

    void LobbyScene::handleEvent(const aer::input::Event &event)
    {
        (void)event;
    }

    void LobbyScene::update(float dt)
    {
        (void)dt;
        const auto rooms = _networkSystem.getAvailableRooms();
        const std::size_t newHash = hashRoomList(rooms);
        if (!_uiBuilt || newHash != _roomsHash) {
            _uiRegistry.clear();
            _uiRegistry.purge();
            _roomsHash = newHash;
            buildUi();
            _uiBuilt = true;
        }

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

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void LobbyScene::buildUi(void)
    {
        float y = 230.0f;
        int shown = 0;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {390.0f, 60.0f},
            makeText("LOBBY - ROOMS", "assets/fonts/title.ttf", 52),
            10,
            {255, 200, 100}
        );

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {120.0f, 140.0f},
            {220.0f, 55.0f},
            makeText("REFRESH", "assets/fonts/main.ttf", 22),
            [this]() {
                _networkSystem.requestListRooms();
                if (_changeScene) {
                    _changeScene(SceneId::Lobby);
                }
            }
        );

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {360.0f, 140.0f},
            {260.0f, 55.0f},
            makeText("CREATE ROOM", "assets/fonts/main.ttf", 22),
            [this]() {
                if (_changeScene) {
                    _changeScene(SceneId::CreateRoom);
                }
            }
        );

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {650.0f, 140.0f},
            {220.0f, 55.0f},
            makeText("BACK", "assets/fonts/main.ttf", 22),
            [this]() {
                if (_changeScene) {
                    _changeScene(SceneId::Menu);
                }
            }
        );

        const auto rooms = _networkSystem.getAvailableRooms();
        if (rooms.empty()) {
            aer::ui::UiFactory::createText(
                _uiRegistry,
                {460.0f, 260.0f},
                makeText("No rooms available", "assets/fonts/main.ttf", 22),
                10
            );
            return;
        }

        for (const auto& room : rooms) {
            if (shown >= 8) {
                break;
            }

            std::string roomInfo = "Room " + std::to_string(room.roomId) +
                                   " | Players: " + std::to_string(room.currentPlayers) +
                                   "/" + std::to_string(room.maxPlayers) +
                                   " | Difficulty: " + std::to_string(room.difficulty);

            aer::ui::UiFactory::createText(
                _uiRegistry,
                {140.0f, y + 10.0f},
                makeText(roomInfo, "assets/fonts/main.ttf", 22),
                10
            );

            aer::ui::UiFactory::createButton(
                _uiRegistry,
                {900.0f, y},
                {140.0f, 50.0f},
                makeText("JOIN", "assets/fonts/main.ttf", 22),
                [this, room]() {
                    _uiSelectedRoomId = room.roomId;
                    _networkSystem.tryJoinRoom(room.roomId, false);
                }
            );

            aer::ui::UiFactory::createButton(
                _uiRegistry,
                {1060.0f, y},
                {120.0f, 50.0f},
                makeText("SPEC", "assets/fonts/main.ttf", 22),
                [this, room]() {
                    _uiSelectedRoomId = room.roomId;
                    _networkSystem.tryJoinRoom(room.roomId, true);
                }
            );

            y += 60.0f;
            ++shown;
        }
    }
} // namespace rtp::client::scenes
