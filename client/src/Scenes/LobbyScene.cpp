/**
 * File   : LobbyScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/LobbyScene.hpp"
#include "RType/ECS/Components/UI/Button.hpp"

#include <cstddef>
#include <functional>
#include <list>
#include <algorithm>
#include <sstream>
#include <iomanip>
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
            auto styleButton = [this](ecs::Entity entity,
                                      const ecs::components::ui::Button &style) {
                auto buttonsOpt = _uiRegistry.get<ecs::components::ui::Button>();
                if (!buttonsOpt) {
                    return;
                }
                auto &buttons = buttonsOpt.value().get();
                if (!buttons.has(entity)) {
                    return;
                }
                auto &button = buttons[entity];
                std::copy(std::begin(style.idleColor), std::end(style.idleColor),
                          std::begin(button.idleColor));
                std::copy(std::begin(style.hoverColor), std::end(style.hoverColor),
                          std::begin(button.hoverColor));
                std::copy(std::begin(style.pressedColor), std::end(style.pressedColor),
                          std::begin(button.pressedColor));
            };

            auto formatFloat = [](float value) -> std::string {
                std::ostringstream oss;
                oss << std::fixed << std::setprecision(1) << value;
                return oss.str();
            };

            ecs::components::ui::Button panelStyle;
            panelStyle.idleColor[0] = 22;
            panelStyle.idleColor[1] = 26;
            panelStyle.idleColor[2] = 34;
            std::copy(std::begin(panelStyle.idleColor), std::end(panelStyle.idleColor),
                      std::begin(panelStyle.hoverColor));
            std::copy(std::begin(panelStyle.idleColor), std::end(panelStyle.idleColor),
                      std::begin(panelStyle.pressedColor));

            ecs::components::ui::Button primaryStyle;
            primaryStyle.idleColor[0] = 32;
            primaryStyle.idleColor[1] = 140;
            primaryStyle.idleColor[2] = 140;
            primaryStyle.hoverColor[0] = 48;
            primaryStyle.hoverColor[1] = 170;
            primaryStyle.hoverColor[2] = 170;
            primaryStyle.pressedColor[0] = 20;
            primaryStyle.pressedColor[1] = 110;
            primaryStyle.pressedColor[2] = 110;

            ecs::components::ui::Button secondaryStyle;
            secondaryStyle.idleColor[0] = 70;
            secondaryStyle.idleColor[1] = 74;
            secondaryStyle.idleColor[2] = 84;
            secondaryStyle.hoverColor[0] = 95;
            secondaryStyle.hoverColor[1] = 100;
            secondaryStyle.hoverColor[2] = 112;
            secondaryStyle.pressedColor[0] = 55;
            secondaryStyle.pressedColor[1] = 60;
            secondaryStyle.pressedColor[2] = 70;

            ecs::components::ui::Button joinStyle;
            joinStyle.idleColor[0] = 40;
            joinStyle.idleColor[1] = 130;
            joinStyle.idleColor[2] = 120;
            joinStyle.hoverColor[0] = 60;
            joinStyle.hoverColor[1] = 160;
            joinStyle.hoverColor[2] = 150;
            joinStyle.pressedColor[0] = 30;
            joinStyle.pressedColor[1] = 100;
            joinStyle.pressedColor[2] = 90;

            ecs::components::ui::Button specStyle;
            specStyle.idleColor[0] = 140;
            specStyle.idleColor[1] = 110;
            specStyle.idleColor[2] = 40;
            specStyle.hoverColor[0] = 180;
            specStyle.hoverColor[1] = 140;
            specStyle.hoverColor[2] = 60;
            specStyle.pressedColor[0] = 120;
            specStyle.pressedColor[1] = 90;
            specStyle.pressedColor[2] = 30;

            ecs::components::ui::Button rowStyleA;
            rowStyleA.idleColor[0] = 28;
            rowStyleA.idleColor[1] = 30;
            rowStyleA.idleColor[2] = 36;
            std::copy(std::begin(rowStyleA.idleColor), std::end(rowStyleA.idleColor),
                      std::begin(rowStyleA.hoverColor));
            std::copy(std::begin(rowStyleA.idleColor), std::end(rowStyleA.idleColor),
                      std::begin(rowStyleA.pressedColor));

            ecs::components::ui::Button rowStyleB;
            rowStyleB.idleColor[0] = 32;
            rowStyleB.idleColor[1] = 35;
            rowStyleB.idleColor[2] = 42;
            std::copy(std::begin(rowStyleB.idleColor), std::end(rowStyleB.idleColor),
                      std::begin(rowStyleB.hoverColor));
            std::copy(std::begin(rowStyleB.idleColor), std::end(rowStyleB.idleColor),
                      std::begin(rowStyleB.pressedColor));

            _uiFactory.createText(
                _uiRegistry,
                {404.0f, 44.0f},
                "LOBBY - ROOMS",
                "assets/fonts/title.ttf",
                52,
                9,
                {12, 14, 18}
            );

            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 40.0f},
                "LOBBY - ROOMS",
                "assets/fonts/title.ttf",
                52,
                10,
                {255, 200, 100}
            );

            const auto &rooms = _network.getAvailableRooms();
            _uiFactory.createText(
                _uiRegistry,
                {450.0f, 110.0f},
                "Rooms available: " + std::to_string(rooms.size()),
                "assets/fonts/main.ttf",
                18,
                10,
                {170, 180, 190}
            );

            const float topY = 150.0f;
            const float topX = 180.0f;
            const float topBtnW = 200.0f;
            const float topBtnH = 50.0f;
            const float topGap = 20.0f;

            auto refreshButton = _uiFactory.createButton(
                _uiRegistry,
                {topX, topY},
                {topBtnW, topBtnH},
                "REFRESH",
                [this]() {
                    _network.requestListRooms();
                    _changeState(GameState::Lobby);
                }
            );
            styleButton(refreshButton, secondaryStyle);

            auto createRoomButton = _uiFactory.createButton(
                _uiRegistry,
                {topX + topBtnW + topGap, topY},
                {topBtnW, topBtnH},
                "CREATE ROOM",
                [this]() {
                    _changeState(GameState::CreateRoom);
                }
            );
            styleButton(createRoomButton, primaryStyle);

            auto backButton = _uiFactory.createButton(
                _uiRegistry,
                {topX + (topBtnW + topGap) * 2.0f, topY},
                {topBtnW, topBtnH},
                "BACK",
                [this]() {
                    _changeState(GameState::Menu);
                }
            );
            styleButton(backButton, secondaryStyle);

            const float panelX = 140.0f;
            const float panelY = 220.0f;
            const float panelW = 1000.0f;
            const float panelH = 420.0f;

            auto listPanel = _uiFactory.createButton(
                _uiRegistry,
                {panelX, panelY},
                {panelW, panelH},
                "",
                nullptr
            );
            styleButton(listPanel, panelStyle);

            _uiFactory.createText(
                _uiRegistry,
                {panelX + 20.0f, panelY + 12.0f},
                "ROOMS",
                "assets/fonts/main.ttf",
                16,
                10,
                {150, 160, 170}
            );

            _uiFactory.createText(
                _uiRegistry,
                {panelX + panelW - 250.0f, panelY + 12.0f},
                "ACTIONS",
                "assets/fonts/main.ttf",
                16,
                10,
                {150, 160, 170}
            );

            float y = panelY + 40.0f;
            int shown = 0;

            for (const auto& room : rooms) {
                if (shown >= 6) {
                    break;
                }

                const float rowY = y + static_cast<float>(shown) * 64.0f;
                auto rowBg = _uiFactory.createButton(
                    _uiRegistry,
                    {panelX + 20.0f, rowY},
                    {panelW - 40.0f, 56.0f},
                    "",
                    nullptr
                );
                styleButton(rowBg, (shown % 2 == 0) ? rowStyleA : rowStyleB);

                std::string roomName = std::string(room.roomName);
                if (roomName.empty()) {
                    roomName = "Room " + std::to_string(room.roomId);
                }

                std::string line1 = roomName + "  •  " +
                                    std::to_string(room.currentPlayers) + "/" +
                                    std::to_string(room.maxPlayers) + " players";

                std::string line2 = "Difficulty " + formatFloat(room.difficulty) +
                                    "  •  Speed " + formatFloat(room.speed) +
                                    "  •  Level " + std::to_string(room.levelId);

                _uiFactory.createText(
                    _uiRegistry,
                    {panelX + 40.0f, rowY + 8.0f},
                    line1,
                    "assets/fonts/main.ttf",
                    20,
                    10,
                    {220, 230, 240}
                );

                _uiFactory.createText(
                    _uiRegistry,
                    {panelX + 40.0f, rowY + 32.0f},
                    line2,
                    "assets/fonts/main.ttf",
                    16,
                    10,
                    {150, 160, 170}
                );

                const float joinW = 110.0f;
                const float specW = 110.0f;
                const float actionY = rowY + 10.0f;
                const float joinX = panelX + panelW - 250.0f;
                const float specX = joinX + joinW + 10.0f;

                auto joinButton = _uiFactory.createButton(
                    _uiRegistry,
                    {joinX, actionY},
                    {joinW, 36.0f},
                    "JOIN",
                    [this, room]() {
                        _uiSelectedRoomId = room.roomId;
                        log::info("Attempting to join Room ID {}", room.roomId);
                        _network.tryJoinRoom(room.roomId, false);
                    }
                );
                styleButton(joinButton, joinStyle);

                auto specButton = _uiFactory.createButton(
                    _uiRegistry,
                    {specX, actionY},
                    {specW, 36.0f},
                    "SPEC",
                    [this, room]() {
                        _uiSelectedRoomId = room.roomId;
                        log::info("Attempting to spectate Room ID {}", room.roomId);
                        _network.tryJoinRoom(room.roomId, true);
                    }
                );
                styleButton(specButton, specStyle);

                if (room.inGame) {
                    _uiFactory.createText(
                        _uiRegistry,
                        {panelX + panelW - 360.0f, rowY + 18.0f},
                        "IN GAME",
                        "assets/fonts/main.ttf",
                        14,
                        10,
                        {220, 120, 90}
                    );
                }

                ++shown;
            }

            if (shown == 0) {
                _uiFactory.createText(
                    _uiRegistry,
                    {panelX + 40.0f, panelY + 80.0f},
                    "No rooms found. Create one or refresh.",
                    "assets/fonts/main.ttf",
                    18,
                    10,
                    {150, 160, 170}
                );
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
