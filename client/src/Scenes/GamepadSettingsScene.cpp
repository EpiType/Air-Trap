/**
 * File   : GamepadSettingsScene.cpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 13/01/2026
 */

#include "Scenes/GamepadSettingsScene.hpp"
#include <SFML/Window/Joystick.hpp>

namespace rtp::client {
    namespace scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        GamepadSettingsScene::GamepadSettingsScene(ecs::Registry& UiRegistry,
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

        void GamepadSettingsScene::onEnter(void)
        {
            log::info("Entering GamepadSettingsScene");
            float yPos = 120.0f;

            // Title
            _uiFactory.createText(
                _uiRegistry,
                {420.0f, 30.0f},
                _translationManager.get("settings.gamepad_settings"),
                "assets/fonts/main.ttf",
                50,
                10,
                {255, 200, 100}
            );

            // Enable/Disable gamepad
            _uiFactory.createText(
                _uiRegistry,
                {240.0f, yPos},
                _translationManager.get("settings.gamepad"),
                "assets/fonts/main.ttf",
                24
            );

            std::vector<std::string> enableOptions = {_translationManager.get("settings.disabled"), _translationManager.get("settings.enabled")};
            _uiFactory.createDropdown(
                _uiRegistry,
                {540.0f, yPos},
                {300.0f, 40.0f},
                enableOptions,
                _settings.getGamepadEnabled() ? 1 : 0,
                [this](int index) {
                    _settings.setGamepadEnabled(index == 1);
                    log::info("Gamepad {}", index == 1 ? "enabled" : "disabled");
                }
            );

            yPos += 70.0f;

            // Deadzone slider
            _uiFactory.createText(
                _uiRegistry,
                {240.0f, yPos},
                _translationManager.get("settings.gamepad_deadzone"),
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createSlider(
                _uiRegistry,
                {540.0f, yPos + 3.0f},
                {300.0f, 20.0f},
                0.0f,
                50.0f,
                _settings.getGamepadDeadzone(),
                [this](float value) {
                    _settings.setGamepadDeadzone(value);
                }
            );

            yPos += 70.0f;

            // Shoot Button
            _uiFactory.createText(
                _uiRegistry,
                {240.0f, yPos + 5.0f},
                _translationManager.get("keybindings.shoot"),
                "assets/fonts/main.ttf",
                24
            );

            unsigned int shootButton = _settings.getGamepadShootButton();
            auto shootEntity = _uiFactory.createButton(
                _uiRegistry,
                {540.0f, yPos},
                {300.0f, 40.0f},
                getButtonName(shootButton),
                [this]() {
                    _isWaitingForButton = true;
                    _actionToRebind = GamepadAction::Shoot;
                    refreshButtonLabel(GamepadAction::Shoot);
                }
            );
            _actionToButton[GamepadAction::Shoot] = shootEntity;

            yPos += 60.0f;

            // Reload Button
            _uiFactory.createText(
                _uiRegistry,
                {240.0f, yPos + 5.0f},
                _translationManager.get("keybindings.reload"),
                "assets/fonts/main.ttf",
                24
            );

            unsigned int reloadButton = _settings.getGamepadReloadButton();
            auto reloadEntity = _uiFactory.createButton(
                _uiRegistry,
                {540.0f, yPos},
                {300.0f, 40.0f},
                getButtonName(reloadButton),
                [this]() {
                    _isWaitingForButton = true;
                    _actionToRebind = GamepadAction::Reload;
                    refreshButtonLabel(GamepadAction::Reload);
                }
            );
            _actionToButton[GamepadAction::Reload] = reloadEntity;

            yPos += 60.0f;

            // Pause Button
            _uiFactory.createText(
                _uiRegistry,
                {240.0f, yPos + 5.0f},
                _translationManager.get("keybindings.pause"),
                "assets/fonts/main.ttf",
                24
            );

            unsigned int pauseButton = _settings.getGamepadPauseButton();
            auto pauseEntity = _uiFactory.createButton(
                _uiRegistry,
                {540.0f, yPos},
                {300.0f, 40.0f},
                getButtonName(pauseButton),
                [this]() {
                    _isWaitingForButton = true;
                    _actionToRebind = GamepadAction::Pause;
                    refreshButtonLabel(GamepadAction::Pause);
                }
            );
            _actionToButton[GamepadAction::Pause] = pauseEntity;

            yPos += 80.0f;

            // Info text
            _uiFactory.createText(
                _uiRegistry,
                {350.0f, yPos},
                "Movement: Left Stick or D-pad",
                "assets/fonts/main.ttf",
                18,
                0,
                {200, 200, 200}
            );

            yPos += 40.0f;

            // Back button
            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, yPos},
                {300.0f, 50.0f},
                _translationManager.get("settings.close"),
                [this]() {
                    _settings.save();
                    _changeState(GameState::Settings);
                }
            );
        }

        void GamepadSettingsScene::onExit(void)
        {
            _isWaitingForButton = false;
        }

        void GamepadSettingsScene::handleEvent(const sf::Event& event)
        {
            if (!_isWaitingForButton)
                return;

            // Check if any gamepad button is pressed
            if (_settings.getGamepadEnabled() && sf::Joystick::isConnected(0)) {
                for (unsigned int i = 0; i < sf::Joystick::getButtonCount(0); ++i) {
                    if (sf::Joystick::isButtonPressed(0, i)) {
                        // Button pressed, assign it
                        switch (_actionToRebind) {
                            case GamepadAction::Shoot:
                                _settings.setGamepadShootButton(i);
                                break;
                            case GamepadAction::Reload:
                                _settings.setGamepadReloadButton(i);
                                break;
                            case GamepadAction::Pause:
                                _settings.setGamepadPauseButton(i);
                                break;
                        }
                        
                        _settings.save();
                        _isWaitingForButton = false;
                        
                        log::info("Gamepad button {} bound to action",
                                    getButtonName(i));
                        
                        refreshButtonLabel(_actionToRebind);
                        
                        // Wait a bit to avoid detecting the same press multiple times
                        sf::sleep(sf::milliseconds(200));
                        return;
                    }
                }
            }
            
            // Allow Escape to cancel
            if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    _isWaitingForButton = false;
                    log::info("Button binding cancelled");
                    refreshButtonLabel(_actionToRebind);
                    return;
                }
            }
        }

        void GamepadSettingsScene::update(float dt)
        {
            (void)dt;
        }

        //////////////////////////////////////////////////////////////////////////
        // Private API
        //////////////////////////////////////////////////////////////////////////

        void GamepadSettingsScene::refreshButtonLabel(GamepadAction action)
        {
            auto it = _actionToButton.find(action);
            if (it == _actionToButton.end()) {
                log::warning("No button found for action");
                return;
            }

            const ecs::Entity e = it->second;

            unsigned int button = 0;
            switch (action) {
                case GamepadAction::Shoot:
                    button = _settings.getGamepadShootButton();
                    break;
                case GamepadAction::Reload:
                    button = _settings.getGamepadReloadButton();
                    break;
                case GamepadAction::Pause:
                    button = _settings.getGamepadPauseButton();
                    break;
            }

            const std::string buttonName = _isWaitingForButton ? "Press button..." : getButtonName(button);

            auto buttonsRes = _uiRegistry.get<ecs::components::ui::Button>();
            if (!buttonsRes) {
                log::error("Failed to get Button components");
                return;
            }

            auto& buttons = buttonsRes.value().get();

            if (!buttons.has(e)) {
                log::warning("Button component not found for entity");
                return;
            }

            buttons[e].text = buttonName;
        }

        std::string GamepadSettingsScene::getButtonName(unsigned int button) const
        {
            static const std::vector<std::string> buttonNames = {
                "A (0)", "B (1)", "X (2)", "Y (3)",
                "LB (4)", "RB (5)", "Back (6)", "Start (7)",
                "LS (8)", "RS (9)", "LT (10)", "RT (11)"
            };
            
            if (button < buttonNames.size()) {
                return buttonNames[button];
            }
            return "Button " + std::to_string(button);
        }

    } // namespace scenes
} // namespace rtp::client
