/**
 * File   : PlayingScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/PlayingScene.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Slider.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/UI/TextInput.hpp"

#include <algorithm>

namespace rtp::client {
    namespace Scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        PlayingScene::PlayingScene(ecs::Registry& worldRegistry,
                                    ecs::Registry& uiRegistry,
                                    Settings& settings,
                                    TranslationManager& translationManager,
                                    NetworkSyncSystem& network,
                                    graphics::UiFactory& uiFactory,
                                    EntityBuilder& worldBuilder,
                                    std::function<void(GameState)> changeState)
            : _uiRegistry(uiRegistry),
              _settings(settings),
              _translationManager(translationManager),
              _network(network),
              _uiFactory(uiFactory),
              _changeState(changeState),
              _worldRegistry(worldRegistry),
              _worldBuilder(worldBuilder)
        {
        }

        void PlayingScene::onEnter()
        {
            rtp::log::info("Entering PlayingScene");

            spawnParallax();

            _chatCompactPanel = _uiFactory.createButton(
                _uiRegistry,
                {30.0f, 15.0f},
                {715.0f, 40.0f},
                "",
                nullptr
            );
            if (auto buttonsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::Button>()) {
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
                {45.0f, 22.0f},
                "",
                "assets/fonts/main.ttf",
                18,
                10,
                {220, 220, 220}
            );

            auto makeHudText = [&](const rtp::Vec2f& pos, unsigned size) -> rtp::ecs::Entity {
                auto eRes = _uiRegistry.spawnEntity();
                if (!eRes) return {};

                auto e = eRes.value();
                rtp::ecs::components::ui::Text t;
                t.content = "";
                t.position = pos;
                t.fontPath = "assets/fonts/main.ttf";
                t.fontSize = size;
                t.red = 255; t.green = 255; t.blue = 255;
                t.alpha = 255;
                t.zIndex = 999;
                _uiRegistry.addComponent<rtp::ecs::components::ui::Text>(e, t);
                return e;
            };

            _hudPing     = makeHudText({1020.0f, 620.0f}, 14);
            _hudFps      = makeHudText({1020.0f, 640.0f}, 14);
            _hudScore    = makeHudText({1020.0f, 660.0f}, 14);
            _hudEntities = makeHudText({1020.0f, 680.0f}, 14);
            _hudAmmo     = makeHudText({1020.0f, 700.0f}, 14);

            _hudChargeBar = _uiFactory.createSlider(
                _uiRegistry,
                {520.0f, 700.0f},
                {240.0f, 6.0f},
                0.0f,
                1.0f,
                0.0f,
                nullptr
            );
            if (auto slidersOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::Slider>()) {
                auto &sliders = slidersOpt.value().get();
                if (sliders.has(_hudChargeBar)) {
                    auto &slider = sliders[_hudChargeBar];
                    slider.trackColor[0] = 25; slider.trackColor[1] = 25; slider.trackColor[2] = 25;
                    slider.fillColor[0] = 180; slider.fillColor[1] = 120; slider.fillColor[2] = 60;
                    slider.handleColor[0] = 25; slider.handleColor[1] = 25; slider.handleColor[2] = 25;
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
        }


        void PlayingScene::handleEvent(const sf::Event& e)
        {
            if (const auto* kp = e.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Enter) {
                    if (!_chatOpen) {
                        openChat();
                    } else {
                        auto inputsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::TextInput>();
                        bool focused = false;
                        if (inputsOpt) {
                            auto &inputs = inputsOpt.value().get();
                            if (inputs.has(_chatInput)) {
                                focused = inputs[_chatInput].isFocused;
                            }
                        }
                        if (!focused) {
                            closeChat();
                        }
                    }
                } else if (kp->code == sf::Keyboard::Key::Escape) {
                    if (_chatOpen) {
                        closeChat();
                        return;
                    }
                    _changeState(GameState::Paused);
                }
            }
        }

        void PlayingScene::update(float dt)
        {
            if (_network.consumeKicked()) {
                _changeState(GameState::Menu);
                return;
            }
            if (!_hudInit) return;

            _fpsTimer += dt;
            _fpsFrames += 1;
            if (_fpsTimer >= 0.5f) {
                _uiFps = static_cast<uint32_t>(_fpsFrames / _fpsTimer);
                _fpsFrames = 0;
                _fpsTimer = 0.0f;
            }
            _uiPing = _network.getPingMs();

            auto textsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::Text>();
            if (!textsOpt) return;
            auto& texts = textsOpt.value().get();

            if (texts.has(_hudPing))     texts[_hudPing].content = "Ping: " + std::to_string(_uiPing) + " ms";
            if (texts.has(_hudFps))      texts[_hudFps].content  = "FPS: "  + std::to_string(_uiFps);
            if (texts.has(_hudScore))    texts[_hudScore].content = "Score: " + std::to_string(_uiScore);

            auto spritesOpt = _worldRegistry.getComponents<rtp::ecs::components::Sprite>();
            if (spritesOpt && texts.has(_hudEntities)) {
                const std::size_t count = spritesOpt.value().get().size();
                texts[_hudEntities].content = "Entities: " + std::to_string(count + 2);
            }

            if (texts.has(_hudAmmo)) {
                const uint16_t current = _network.getAmmoCurrent();
                const uint16_t max = _network.getAmmoMax();
                std::string ammoText = "Ammo: " + std::to_string(current) + "/" + std::to_string(max);
                if (_network.isReloading()) {
                    const float remaining = _network.getReloadCooldownRemaining();
                    ammoText += " (Reloading " + std::to_string(remaining).substr(0, 4) + "s)";
                }
                texts[_hudAmmo].content = ammoText;
            }

            if (texts.has(_chatCompactText)) {
                texts[_chatCompactText].content = _network.getLastChatMessage();
            }

            if (_chatOpen) {
                updateChatHistoryText();
            }

            auto slidersOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::Slider>();
            if (slidersOpt && slidersOpt->get().has(_hudChargeBar)) {
                constexpr float kChargeMax = 2.0f;
                bool canCharge = !_network.isReloading() && _network.getAmmoCurrent() > 0;

                if (canCharge) {
                    auto inputsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::TextInput>();
                    if (inputsOpt) {
                        auto &inputs = inputsOpt.value().get();
                        for (const auto &e : inputs.getEntities()) {
                            if (inputs[e].isFocused) {
                                canCharge = false;
                                break;
                            }
                        }
                    }
                }

                if (canCharge && sf::Keyboard::isKeyPressed(_settings.getKey(KeyAction::Shoot))) {
                    _chargeTime = std::min(_chargeTime + dt, kChargeMax);
                } else {
                    _chargeTime = 0.0f;
                }

                auto &slider = slidersOpt->get()[_hudChargeBar];
                slider.currentValue = (_chargeTime > 0.0f) ? (_chargeTime / kChargeMax) : 0.0f;
            }
        }

        ////////////////////////////////////////////////////////////////////////
        // Private API
        ////////////////////////////////////////////////////////////////////////

        void PlayingScene::spawnParallax()
        {
            constexpr float baseTextureWidth = 1280.0f;

            auto spawnLayer = [&](EntityTemplate t) {
                const float scaledW = baseTextureWidth * t.scale.x;

                t.position.x = 0.0f;
                auto a = _worldBuilder.spawn(t);
                if (!a) {
                    rtp::log::error("Failed to spawn parallax A");
                    return;
                }

                t.position.x = scaledW;
                auto b = _worldBuilder.spawn(t);
                if (!b) {
                    rtp::log::error("Failed to spawn parallax B");
                    return;
                }
            };

            spawnLayer(EntityTemplate::createParrallaxLayer1());
            spawnLayer(EntityTemplate::createParrallaxLayer2());
        }

        void PlayingScene::sendChatMessage(void)
        {
            auto inputsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::TextInput>();
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
            closeChat();
        }

        void PlayingScene::openChat(void)
        {
            if (_chatOpen)
                return;

            _chatOpen = true;

            _chatPanel = _uiFactory.createButton(
                _uiRegistry,
                {30.0f, 60.0f},
                {715.0f, 220.0f},
                "",
                nullptr
            );
            if (auto buttonsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::Button>()) {
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
                {45.0f, 70.0f},
                "",
                "assets/fonts/main.ttf",
                18,
                10,
                {230, 230, 230}
            );

            _chatInput = _uiFactory.createTextInput(
                _uiRegistry,
                {45.0f, 235.0f},
                {685.0f, 30.0f},
                "assets/fonts/main.ttf",
                18,
                120,
                "Type message...",
                nullptr,
                nullptr
            );
            if (auto inputsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::TextInput>()) {
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
            if (!_chatOpen)
                return;

            _chatOpen = false;
            if (!_chatPanel.isNull()) _uiRegistry.killEntity(_chatPanel);
            if (!_chatHistoryText.isNull()) _uiRegistry.killEntity(_chatHistoryText);
            if (!_chatInput.isNull()) _uiRegistry.killEntity(_chatInput);
            _chatPanel = {};
            _chatHistoryText = {};
            _chatInput = {};
        }

        void PlayingScene::updateChatHistoryText(void)
        {
            auto textsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::Text>();
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

    } // namespace Scenes
} // namespace rtp::client
