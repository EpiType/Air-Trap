/**
 * File   : GamepadSettingsScene.cpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/scenes/GamepadSettingsScene.hpp"

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
    }

    GamepadSettingsScene::GamepadSettingsScene(aer::ecs::Registry& uiRegistry,
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

    void GamepadSettingsScene::onEnter(void)
    {
        float yPos = 120.0f;
        const auto buttonOptions = buildButtonOptions();
        const auto clampIndex = [&buttonOptions](unsigned int value) -> int {
            const unsigned int maxIndex = static_cast<unsigned int>(buttonOptions.size() - 1);
            return static_cast<int>(std::min(value, maxIndex));
        };

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {420.0f, 30.0f},
            makeText(_translation.get("settings.gamepad_settings"), "assets/fonts/main.ttf", 50),
            10,
            {255, 200, 100}
        );

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos},
            makeText(_translation.get("settings.gamepad"), "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createDropdown(
            _uiRegistry,
            {540.0f, yPos},
            {300.0f, 40.0f},
            {_translation.get("settings.disabled"), _translation.get("settings.enabled")},
            _settings.gamepadEnabled() ? 1 : 0,
            [this](int index) { _settings.setGamepadEnabled(index == 1); }
        );

        yPos += 70.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos},
            makeText(_translation.get("settings.gamepad_deadzone"), "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createSlider(
            _uiRegistry,
            {540.0f, yPos + 3.0f},
            {300.0f, 20.0f},
            0.0f,
            50.0f,
            _settings.gamepadDeadzone(),
            [this](float value) { _settings.setGamepadDeadzone(value); }
        );

        yPos += 70.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos + 5.0f},
            makeText(_translation.get("keybindings.shoot"), "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createDropdown(
            _uiRegistry,
            {540.0f, yPos},
            {300.0f, 40.0f},
            buttonOptions,
            clampIndex(_settings.gamepadShootButton()),
            [this](int index) { _settings.setGamepadShootButton(static_cast<unsigned int>(index)); }
        );

        yPos += 60.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos + 5.0f},
            makeText(_translation.get("keybindings.reload"), "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createDropdown(
            _uiRegistry,
            {540.0f, yPos},
            {300.0f, 40.0f},
            buttonOptions,
            clampIndex(_settings.gamepadReloadButton()),
            [this](int index) { _settings.setGamepadReloadButton(static_cast<unsigned int>(index)); }
        );

        yPos += 60.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos + 5.0f},
            makeText(_translation.get("keybindings.pause"), "assets/fonts/main.ttf", 24)
        );

        aer::ui::UiFactory::createDropdown(
            _uiRegistry,
            {540.0f, yPos},
            {300.0f, 40.0f},
            buttonOptions,
            clampIndex(_settings.gamepadPauseButton()),
            [this](int index) { _settings.setGamepadPauseButton(static_cast<unsigned int>(index)); }
        );

        yPos += 80.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {350.0f, yPos},
            makeText("Movement: Left Stick or D-pad", "assets/fonts/main.ttf", 18),
            0,
            {200, 200, 200}
        );

        yPos += 40.0f;

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {490.0f, yPos},
            {300.0f, 50.0f},
            makeText(_translation.get("settings.close"), "assets/fonts/main.ttf", 22),
            [this]() {
                _settings.save();
                if (_changeScene) {
                    _changeScene(SceneId::Settings);
                }
            }
        );
    }

    void GamepadSettingsScene::onExit(void)
    {
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void GamepadSettingsScene::handleEvent(const aer::input::Event &event)
    {
        if (event.type == aer::input::EventType::KeyDown &&
            event.key == aer::input::KeyCode::Escape) {
            if (_changeScene) {
                _changeScene(SceneId::Settings);
            }
        }
    }

    void GamepadSettingsScene::update(float dt)
    {
        (void)dt;
        (void)_networkSystem;
    }

    std::vector<std::string> GamepadSettingsScene::buildButtonOptions()
    {
        return {
            "A (0)",
            "B (1)",
            "X (2)",
            "Y (3)",
            "LB (4)",
            "RB (5)",
            "Back (6)",
            "Start (7)",
            "LS (8)",
            "RS (9)",
            "LT (10)",
            "RT (11)"
        };
    }
} // namespace rtp::client::scenes
