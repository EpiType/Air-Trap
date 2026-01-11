/**
 * File   : KeyBindingScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/KeyBindingScene.hpp"

namespace rtp::client {
    namespace Scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        KeyBindingScene::KeyBindingScene(ecs::Registry& UiRegistry,
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

        void KeyBindingScene::onEnter(void)
        {
            rtp::log::info("Entering KeyBindingScene");

            _uiFactory.createText(
                _uiRegistry,
                {450.0f, 50.0f},
                _translationManager.get("keybindings.title"),
                "assets/fonts/main.ttf",
                60,
                10,
                {255, 200, 100}
            );

            float yPos = 150.0f;
            const float buttonSpacing = 60.0f;

            struct KeyBindingButton {
                    const char *labelKey;
                    KeyAction action;
            };

            std::vector<KeyBindingButton> bindings = {
                {"keybindings.move_up",    KeyAction::MoveUp   },
                {"keybindings.move_down",  KeyAction::MoveDown },
                {"keybindings.move_left",  KeyAction::MoveLeft },
                {"keybindings.move_right", KeyAction::MoveRight},
                {"keybindings.shoot",      KeyAction::Shoot    },
                {"keybindings.pause",      KeyAction::Pause    }
            };

            for (const auto& binding : bindings) {
                _uiFactory.createText(
                    _uiRegistry,
                    {200.0f, yPos + 10.0f},
                    _translationManager.get(binding.labelKey),
                    "assets/fonts/main.ttf",
                    24,
                    22,
                    {255, 255, 255}
                );
                sf::Keyboard::Key currentKey = _settings.getKey(binding.action);
                _uiFactory.createButton(
                    _uiRegistry,
                    {500.0f, yPos},
                    {200.0f, 40.0f},
                    _settings.getKeyName(currentKey),
                    [this, actionToBind = binding.action]() {
                        _isWaitingForKey = true;
                        // _keyActionToRebind = actionToBind;
                    }
                );
                yPos += buttonSpacing;
            }

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 650.0f},
                {300.0f, 60.0f},
                _translationManager.get("settings.back"),
                [this]() {
                    _settings.save();
                    _changeState(GameState::Settings);
                }
            );
        }

        void KeyBindingScene::onExit(void)
        {
        }

        void KeyBindingScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void KeyBindingScene::update(float dt)
        {
            (void)dt;
        }

    } // namespace Scenes
} // namespace rtp::client