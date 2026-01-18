/**
 * File   : RoomWaitingScene.cpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/scenes/RoomWaitingScene.hpp"

#include "engine/ecs/components/ui/Button.hpp"
#include "engine/ecs/components/ui/Text.hpp"
#include "engine/ecs/components/ui/TextInput.hpp"

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

        void setPanelColors(aer::ecs::components::Button &panel)
        {
            panel.idleColor = {20, 20, 20};
            panel.hoverColor = {20, 20, 20};
            panel.pressedColor = {20, 20, 20};
        }
    }

    RoomWaitingScene::RoomWaitingScene(aer::ecs::Registry& uiRegistry,
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

    void RoomWaitingScene::onEnter(void)
    {
        _chatCompactPanel = aer::ui::UiFactory::createButton(
            _uiRegistry,
            {30.0f, 20.0f},
            {900.0f, 40.0f},
            makeText("", "assets/fonts/main.ttf", 18),
            nullptr
        );
        if (auto buttonsOpt = _uiRegistry.getComponents<aer::ecs::components::Button>()) {
            auto &buttons = buttonsOpt.value().get();
            if (buttons.has(_chatCompactPanel)) {
                setPanelColors(buttons[_chatCompactPanel]);
            }
        }

        _chatCompactText = aer::ui::UiFactory::createText(
            _uiRegistry,
            {45.0f, 28.0f},
            makeText("", "assets/fonts/main.ttf", 20),
            10,
            {220, 220, 220}
        );

        _chatToggleButton = aer::ui::UiFactory::createButton(
            _uiRegistry,
            {940.0f, 20.0f},
            {300.0f, 40.0f},
            makeText("CHAT", "assets/fonts/main.ttf", 20),
            [this]() {
                if (_chatOpen) {
                    closeChat();
                } else {
                    openChat();
                }
            }
        );

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {390.0f, 140.0f},
            makeText("Waiting for players...", "assets/fonts/title.ttf", 56),
            10,
            {255, 200, 100}
        );

        _readyButton = aer::ui::UiFactory::createButton(
            _uiRegistry,
            {420.0f, 280.0f},
            {420.0f, 60.0f},
            makeText("Ready", "assets/fonts/main.ttf", 22),
            [this]() {
                _uiReady = !_uiReady;
                _networkSystem.trySetReady(_uiReady);
                updateReadyButtonText();
            }
        );
        updateReadyButtonText();

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {420.0f, 360.0f},
            {420.0f, 60.0f},
            makeText("Leave Room", "assets/fonts/main.ttf", 22),
            [this]() {
                _networkSystem.tryLeaveRoom();
                if (_changeScene) {
                    _changeScene(SceneId::Lobby);
                }
            }
        );
    }

    void RoomWaitingScene::onExit(void)
    {
        closeChat();
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void RoomWaitingScene::handleEvent(const aer::input::Event &event)
    {
        (void)event;
    }

    void RoomWaitingScene::update(float dt)
    {
        (void)dt;
        if (_networkSystem.consumeKicked()) {
            if (_changeScene) {
                _changeScene(SceneId::Menu);
            }
            return;
        }
        if (_networkSystem.isInGame()) {
            if (_changeScene) {
                _changeScene(SceneId::Playing);
            }
        }

        auto textsOpt = _uiRegistry.getComponents<aer::ecs::components::Text>();
        if (textsOpt) {
            auto &texts = textsOpt.value().get();
            if (texts.has(_chatCompactText)) {
                texts[_chatCompactText].text.content = _networkSystem.getLastChatMessage();
            }
        }

        if (_chatOpen) {
            updateChatHistoryText();
        }
    }

    void RoomWaitingScene::openChat(void)
    {
        if (_chatOpen) {
            return;
        }

        _chatOpen = true;

        _chatPanel = aer::ui::UiFactory::createButton(
            _uiRegistry,
            {30.0f, 70.0f},
            {1220.0f, 220.0f},
            makeText("", "assets/fonts/main.ttf", 18),
            nullptr
        );
        if (auto buttonsOpt = _uiRegistry.getComponents<aer::ecs::components::Button>()) {
            auto &buttons = buttonsOpt.value().get();
            if (buttons.has(_chatPanel)) {
                setPanelColors(buttons[_chatPanel]);
            }
        }

        _chatHistoryText = aer::ui::UiFactory::createText(
            _uiRegistry,
            {45.0f, 80.0f},
            makeText("", "assets/fonts/main.ttf", 18),
            10,
            {230, 230, 230}
        );

        _chatInput = aer::ui::UiFactory::createTextInput(
            _uiRegistry,
            {45.0f, 250.0f},
            {1180.0f, 30.0f},
            120,
            makeText("Type message...", "assets/fonts/main.ttf", 18),
            nullptr,
            nullptr
        );
        if (auto inputsOpt = _uiRegistry.getComponents<aer::ecs::components::TextInput>()) {
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
        if (!_chatOpen) {
            return;
        }

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
        auto textsOpt = _uiRegistry.getComponents<aer::ecs::components::Text>();
        if (!textsOpt) {
            return;
        }

        auto &texts = textsOpt.value().get();
        if (!texts.has(_chatHistoryText)) {
            return;
        }

        const auto &history = _networkSystem.getChatHistory();
        std::string combined;
        for (const auto &line : history) {
            if (!combined.empty()) {
                combined += "\n";
            }
            combined += line;
        }
        texts[_chatHistoryText].text.content = combined;
    }

    void RoomWaitingScene::sendChatMessage(void)
    {
        auto inputsOpt = _uiRegistry.getComponents<aer::ecs::components::TextInput>();
        if (!inputsOpt) {
            return;
        }

        auto &inputs = inputsOpt.value().get();
        if (!inputs.has(_chatInput)) {
            return;
        }

        auto &input = inputs[_chatInput];
        const std::string message = input.value;
        if (message.empty()) {
            return;
        }

        _networkSystem.trySendMessage(message);
        input.value.clear();
    }

    void RoomWaitingScene::updateReadyButtonText(void)
    {
        auto buttonsOpt = _uiRegistry.getComponents<aer::ecs::components::Button>();
        if (!buttonsOpt || _readyButton.isNull()) {
            return;
        }
        auto &buttons = buttonsOpt.value().get();
        if (!buttons.has(_readyButton)) {
            return;
        }
        buttons[_readyButton].text.content = _uiReady ? "Not Ready" : "Ready";
    }
} // namespace rtp::client::scenes
