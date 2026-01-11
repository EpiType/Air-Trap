/**
 * File   : RoomWaitingScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/RoomWaitingScene.hpp"

namespace rtp::client {
    namespace Scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        RoomWaitingScene::RoomWaitingScene(ecs::Registry& UiRegistry,
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

        void RoomWaitingScene::onEnter(void)
        {
            rtp::log::info("Entering RoomWaitingScene");

            _uiFactory.createText(
                _uiRegistry,
                {390.0f, 80.0f},
                "Waiting for players...",
                "assets/fonts/title.ttf",
                56,
                10,
                {255, 200, 100}
            );

            _uiFactory.createButton(
                _uiRegistry,
                {420.0f, 280.0f},
                {420.0f, 60.0f},
                "Set Ready",
                [this]() {
                    _uiReady = !_uiReady;
                    std::string buttonText = _uiReady ?
                        "Not Ready" :
                        "Ready";

                    rtp::log::info("Player is now {}", _uiReady ? "ready" : "not ready");

                    _network.trySetReady(_uiReady);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {420.0f, 360.0f},
                {420.0f, 60.0f},
                "Leave Room",
                [this]() {
                    _network.tryLeaveRoom();
                    _changeState(GameState::Lobby);
                }
            );
        }

        void RoomWaitingScene::onExit(void)
        {
            _uiRegistry.clear();
        }

        void RoomWaitingScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void RoomWaitingScene::update(float dt)
        {
            (void)dt;
        }
    } // namespace Scenes
} // namespace rtp::client