/**
 * File   : PlayingScene.cpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/scenes/PlayingScene.hpp"

#include "engine/ecs/components/Sprite.hpp"
#include "engine/ecs/components/ui/Button.hpp"
#include "engine/ecs/components/ui/Slider.hpp"
#include "engine/ecs/components/ui/Text.hpp"
#include "engine/ecs/components/ui/TextInput.hpp"
#include "engine/input/Input.hpp"

#include <algorithm>

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

    PlayingScene::PlayingScene(aer::ecs::Registry& worldRegistry,
                               aer::ecs::Registry& uiRegistry,
                               Settings& settings,
                               TranslationManager& translation,
                               NetworkSyncSystem& network,
                               EntityBuilder& worldBuilder,
                               std::function<void(SceneId)> changeScene)
        : _worldRegistry(worldRegistry),
          _uiRegistry(uiRegistry),
          _settings(settings),
          _translation(translation),
          _networkSystem(network),
          _worldBuilder(worldBuilder),
          _changeScene(changeScene)
    {
    }

    void PlayingScene::onEnter(void)
    {
        spawnParallax();

        _chatCompactPanel = aer::ui::UiFactory::createButton(
            _uiRegistry,
            {30.0f, 15.0f},
            {715.0f, 40.0f},
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
            {45.0f, 22.0f},
            makeText("", "assets/fonts/main.ttf", 18),
            10,
            {220, 220, 220}
        );

        auto makeHudText = [&](const aer::math::Vec2f& pos, unsigned size) -> aer::ecs::Entity {
            return aer::ui::UiFactory::createText(
                _uiRegistry,
                pos,
                makeText("", "assets/fonts/main.ttf", size),
                999,
                {255, 255, 255}
            );
        };

        _hudPing = makeHudText({1020.0f, 620.0f}, 14);
        _hudFps = makeHudText({1020.0f, 640.0f}, 14);
        _hudScore = makeHudText({1020.0f, 660.0f}, 14);
        _hudEntities = makeHudText({1020.0f, 680.0f}, 14);
        _hudAmmo = makeHudText({1020.0f, 700.0f}, 14);

        _hudChargeBar = aer::ui::UiFactory::createSlider(
            _uiRegistry,
            {520.0f, 700.0f},
            {240.0f, 6.0f},
            0.0f,
            1.0f,
            0.0f,
            nullptr
        );

        if (auto slidersOpt = _uiRegistry.getComponents<aer::ecs::components::Slider>()) {
            auto &sliders = slidersOpt.value().get();
            if (sliders.has(_hudChargeBar)) {
                auto &slider = sliders[_hudChargeBar];
                slider.trackColor = {25, 25, 25};
                slider.fillColor = {180, 120, 60};
                slider.handleColor = {25, 25, 25};
                slider.zIndex = 999;
            }
        }

        _hudInit = true;
        _chargeTime = 0.0f;
    }

    void PlayingScene::onExit(void)
    {
        _hudInit = false;
        closeChat();
        _chargeTime = 0.0f;
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void PlayingScene::handleEvent(const aer::input::Event &event)
    {
        if (event.type != aer::input::EventType::KeyDown) {
            return;
        }

        if (event.key == aer::input::KeyCode::Return) {
            if (!_chatOpen) {
                openChat();
            } else {
                bool focused = false;
                if (auto inputsOpt = _uiRegistry.getComponents<aer::ecs::components::TextInput>()) {
                    auto &inputs = inputsOpt.value().get();
                    if (inputs.has(_chatInput)) {
                        focused = inputs[_chatInput].isFocused;
                    }
                }
                if (!focused) {
                    closeChat();
                }
            }
            return;
        }

        if (event.key == _settings.getKey(KeyAction::Pause) ||
            event.key == aer::input::KeyCode::Escape) {
            if (_chatOpen) {
                closeChat();
                return;
            }
            if (_changeScene) {
                _changeScene(SceneId::Paused);
            }
        }
    }

    void PlayingScene::update(float dt)
    {
        if (_networkSystem.consumeKicked()) {
            if (_changeScene) {
                _changeScene(SceneId::Menu);
            }
            return;
        }

        if (!_hudInit) {
            return;
        }

        _fpsTimer += dt;
        _fpsFrames += 1;
        if (_fpsTimer >= 0.5f) {
            _uiFps = static_cast<uint32_t>(_fpsFrames / _fpsTimer);
            _fpsFrames = 0;
            _fpsTimer = 0.0f;
        }
        _uiPing = _networkSystem.getPingMs();

        auto textsOpt = _uiRegistry.getComponents<aer::ecs::components::Text>();
        if (textsOpt) {
            auto &texts = textsOpt.value().get();
            if (texts.has(_hudPing)) {
                texts[_hudPing].text.content = "Ping: " + std::to_string(_uiPing) + " ms";
            }
            if (texts.has(_hudFps)) {
                texts[_hudFps].text.content = "FPS: " + std::to_string(_uiFps);
            }
            if (texts.has(_hudScore)) {
                texts[_hudScore].text.content = "Score: " + std::to_string(_uiScore);
            }
        }

        if (auto spritesOpt = _worldRegistry.getComponents<aer::ecs::components::Sprite>()) {
            if (textsOpt) {
                auto &texts = textsOpt.value().get();
                if (texts.has(_hudEntities)) {
                    const std::size_t count = spritesOpt.value().get().size();
                    texts[_hudEntities].text.content = "Entities: " + std::to_string(count + 2);
                }
            }
        }

        if (textsOpt && textsOpt->get().has(_hudAmmo)) {
            const uint16_t current = _networkSystem.getAmmoCurrent();
            const uint16_t max = _networkSystem.getAmmoMax();
            std::string ammoText = "Ammo: " + std::to_string(current) + "/" + std::to_string(max);
            if (_networkSystem.isReloading()) {
                const float remaining = _networkSystem.getReloadCooldownRemaining();
                ammoText += " (Reloading " + std::to_string(remaining).substr(0, 4) + "s)";
            }
            textsOpt->get()[_hudAmmo].text.content = ammoText;
        }

        if (textsOpt && textsOpt->get().has(_chatCompactText)) {
            textsOpt->get()[_chatCompactText].text.content = _networkSystem.getLastChatMessage();
        }

        if (_chatOpen) {
            updateChatHistoryText();
        }

        if (auto slidersOpt = _uiRegistry.getComponents<aer::ecs::components::Slider>()) {
            if (slidersOpt->get().has(_hudChargeBar)) {
                constexpr float kChargeMax = 2.0f;
                bool canCharge = !_networkSystem.isReloading() && _networkSystem.getAmmoCurrent() > 0;

                if (canCharge) {
                    if (auto inputsOpt = _uiRegistry.getComponents<aer::ecs::components::TextInput>()) {
                        auto &inputs = inputsOpt.value().get();
                        for (const auto &e : inputs.entities()) {
                            if (inputs[e].isFocused) {
                                canCharge = false;
                                break;
                            }
                        }
                    }
                }

                const auto &input = aer::input::Input::instance();
                if (canCharge && input.isKeyDown(_settings.getKey(KeyAction::Shoot))) {
                    _chargeTime = std::min(_chargeTime + dt, kChargeMax);
                } else {
                    _chargeTime = 0.0f;
                }

                auto &slider = slidersOpt->get()[_hudChargeBar];
                slider.currentValue = (_chargeTime > 0.0f) ? (_chargeTime / kChargeMax) : 0.0f;
            }
        }
    }

    void PlayingScene::spawnParallax(void)
    {
        constexpr float baseTextureWidth = 1280.0f;

        auto spawnLayer = [&](EntityTemplate t) {
            const float scaledW = baseTextureWidth * t.scale.x;

            t.position.x = 0.0f;
            auto a = _worldBuilder.spawn(t);
            if (!a) {
                return;
            }

            t.position.x = scaledW;
            auto b = _worldBuilder.spawn(t);
            if (!b) {
                return;
            }
        };

        spawnLayer(EntityTemplate::createParrallaxLayer1());
        spawnLayer(EntityTemplate::createParrallaxLayer2());
    }

    void PlayingScene::sendChatMessage(void)
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
        closeChat();
    }

    void PlayingScene::openChat(void)
    {
        if (_chatOpen) {
            return;
        }

        _chatOpen = true;

        _chatPanel = aer::ui::UiFactory::createButton(
            _uiRegistry,
            {30.0f, 60.0f},
            {715.0f, 220.0f},
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
            {45.0f, 70.0f},
            makeText("", "assets/fonts/main.ttf", 18),
            10,
            {230, 230, 230}
        );

        _chatInput = aer::ui::UiFactory::createTextInput(
            _uiRegistry,
            {45.0f, 235.0f},
            {685.0f, 30.0f},
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

    void PlayingScene::closeChat(void)
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

    void PlayingScene::updateChatHistoryText(void)
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
} // namespace rtp::client::scenes
