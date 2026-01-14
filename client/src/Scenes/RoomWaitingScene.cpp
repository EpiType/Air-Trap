/**
 * File   : RoomWaitingScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/RoomWaitingScene.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/UI/TextInput.hpp"

namespace rtp::client {
    namespace scenes {

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
            log::info("Entering RoomWaitingScene");

            _chatCompactPanel = _uiFactory.createButton(
                _uiRegistry,
                {30.0f, 20.0f},
                {900.0f, 40.0f},
                "",
                nullptr
            );
            if (auto buttonsOpt = _uiRegistry.get<ecs::components::ui::Button>()) {
                auto &buttons = buttonsOpt.value().get();
                if (buttons.has(_chatCompactPanel)) {
                    auto &panel = buttons[_chatCompactPanel];
                    panel.idleColor[0] = 20; panel.idleColor[1] = 20; panel.idleColor[2] = 20;
                    panel.hoverColor[0] = 20; panel.hoverColor[1] = 20; panel.hoverColor[2] = 20;
                    panel.pressedColor[0] = 20; panel.pressedColor[1] = 20; panel.pressedColor[2] = 20;
                }
            }

            _chatCompactText = _uiFactory.createText(
                _uiRegistry,
                {45.0f, 28.0f},
                "",
                "assets/fonts/main.ttf",
                20,
                10,
                {220, 220, 220}
            );

            _chatToggleButton = _uiFactory.createButton(
                _uiRegistry,
                {940.0f, 20.0f},
                {300.0f, 40.0f},
                "CHAT",
                [this]() {
                    if (_chatOpen) {
                        closeChat();
                    } else {
                        openChat();
                    }
                }
            );

            _uiFactory.createText(
                _uiRegistry,
                {390.0f, 140.0f},
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

                    log::info("Player is now {}", _uiReady ? "ready" : "not ready");

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
            closeChat();
        }

        void RoomWaitingScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void RoomWaitingScene::update(float dt)
        {
            (void)dt;
            if (_network.consumeKicked()) {
                _changeState(GameState::Menu);
                return;
            }
            if (_network.isInGame()) {
                _changeState(GameState::Playing);
            }

            auto textsOpt = _uiRegistry.get<ecs::components::ui::Text>();
            if (textsOpt) {
                auto &texts = textsOpt.value().get();
                if (texts.has(_chatCompactText)) {
                    texts[_chatCompactText].content = _network.getLastChatMessage();
                }
            }

            if (_chatOpen) {
                updateChatHistoryText();
            }
        }

        void RoomWaitingScene::openChat(void)
        {
            if (_chatOpen)
                return;

            _chatOpen = true;

            _chatPanel = _uiFactory.createButton(
                _uiRegistry,
                {30.0f, 70.0f},
                {1220.0f, 220.0f},
                "",
                nullptr
            );
            if (auto buttonsOpt = _uiRegistry.get<ecs::components::ui::Button>()) {
                auto &buttons = buttonsOpt.value().get();
                if (buttons.has(_chatPanel)) {
                    auto &panel = buttons[_chatPanel];
                    panel.idleColor[0] = 20; panel.idleColor[1] = 20; panel.idleColor[2] = 20;
                    panel.hoverColor[0] = 20; panel.hoverColor[1] = 20; panel.hoverColor[2] = 20;
                    panel.pressedColor[0] = 20; panel.pressedColor[1] = 20; panel.pressedColor[2] = 20;
                }
            }

            _chatHistoryText = _uiFactory.createText(
                _uiRegistry,
                {45.0f, 80.0f},
                "",
                "assets/fonts/main.ttf",
                18,
                10,
                {230, 230, 230}
            );

            _chatInput = _uiFactory.createTextInput(
                _uiRegistry,
                {45.0f, 250.0f},
                {1180.0f, 30.0f},
                "assets/fonts/main.ttf",
                18,
                120,
                "Type message...",
                nullptr,
                nullptr
            );
            if (auto inputsOpt = _uiRegistry.get<ecs::components::ui::TextInput>()) {
                auto &inputs = inputsOpt.value().get();
                if (inputs.has(_chatInput)) {
                    inputs[_chatInput].onSubmit = [this](const std::string&) {
                        sendChatMessage();
                    };
                    inputs[_chatInput].isFocused = true;
                    inputs[_chatInput].showCursor = true;
                }
            }

            updateChatHistoryText();
        }

        void RoomWaitingScene::closeChat(void)
        {
            if (!_chatOpen)
                return;

            _chatOpen = false;
            if (!_chatPanel.isNull()) _uiRegistry.kill(_chatPanel);
            if (!_chatHistoryText.isNull()) _uiRegistry.kill(_chatHistoryText);
            if (!_chatInput.isNull()) _uiRegistry.kill(_chatInput);
            _chatPanel = {};
            _chatHistoryText = {};
            _chatInput = {};
        }

        void RoomWaitingScene::updateChatHistoryText(void)
        {
            auto textsOpt = _uiRegistry.get<ecs::components::ui::Text>();
            if (!textsOpt)
                return;
            auto &texts = textsOpt.value().get();
            if (!texts.has(_chatHistoryText))
                return;

            const auto &history = _network.getChatHistory();
            std::string combined;
            for (const auto &line : history) {
                if (!combined.empty())
                    combined += "\n";
                combined += line;
            }
            texts[_chatHistoryText].content = combined;
        }

        void RoomWaitingScene::sendChatMessage(void)
        {
            auto inputsOpt = _uiRegistry.get<ecs::components::ui::TextInput>();
            if (!inputsOpt)
                return;

            auto &inputs = inputsOpt.value().get();
            if (!inputs.has(_chatInput))
                return;

            auto &input = inputs[_chatInput];
            const std::string message = input.value;
            if (message.empty())
                return;

            _network.trySendMessage(message);
            input.value.clear();
        }
    } // namespace scenes
} // namespace rtp::client
