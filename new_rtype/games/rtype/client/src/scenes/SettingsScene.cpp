/**
 * File   : SettingsScene.cpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/scenes/SettingsScene.hpp"

#include <vector>

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

    SettingsScene::SettingsScene(aer::ecs::Registry& uiRegistry,
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

    void SettingsScene::onEnter(void)
    {
        float yPos = 120.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {540.0f, 30.0f},
            makeText(_translation.get("settings.title"), "assets/fonts/main.ttf", 50),
            10,
            {255, 200, 100}
        );

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos},
            makeText(_translation.get("settings.master_volume"), "assets/fonts/main.ttf", 22)
        );

        aer::ui::UiFactory::createSlider(
            _uiRegistry,
            {540.0f, yPos + 3.0f},
            {300.0f, 15.0f},
            0.0f,
            1.0f,
            _settings.masterVolume(),
            [this](float value) { _settings.setMasterVolume(value); }
        );

        yPos += 45.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos},
            makeText(_translation.get("settings.music_volume"), "assets/fonts/main.ttf", 22)
        );

        aer::ui::UiFactory::createSlider(
            _uiRegistry,
            {540.0f, yPos + 3.0f},
            {300.0f, 15.0f},
            0.0f,
            1.0f,
            _settings.musicVolume(),
            [this](float value) { _settings.setMusicVolume(value); }
        );

        yPos += 45.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {290.0f, yPos},
            makeText(_translation.get("settings.sfx_volume"), "assets/fonts/main.ttf", 22)
        );

        aer::ui::UiFactory::createSlider(
            _uiRegistry,
            {540.0f, yPos + 3.0f},
            {300.0f, 15.0f},
            0.0f,
            1.0f,
            _settings.sfxVolume(),
            [this](float value) { _settings.setSfxVolume(value); }
        );

        yPos += 60.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos},
            makeText(_translation.get("settings.language"), "assets/fonts/main.ttf", 22)
        );

        aer::ui::UiFactory::createDropdown(
            _uiRegistry,
            {540.0f, yPos},
            {300.0f, 35.0f},
            {"English", "Français", "Español", "Deutsch", "Italiano"},
            static_cast<int>(_settings.language()),
            [this](int index) {
                _settings.setLanguage(static_cast<Settings::Language>(index));
            }
        );

        yPos += 55.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos},
            makeText(_translation.get("settings.colorblind_mode"), "assets/fonts/main.ttf", 22)
        );

        std::vector<std::string> colorblindOptions = {
            _translation.get("colorblind.none"),
            _translation.get("colorblind.protanopia"),
            _translation.get("colorblind.deuteranopia"),
            _translation.get("colorblind.tritanopia")
        };

        aer::ui::UiFactory::createDropdown(
            _uiRegistry,
            {540.0f, yPos},
            {300.0f, 35.0f},
            colorblindOptions,
            static_cast<int>(_settings.colorBlindMode()),
            [this](int index) {
                _settings.setColorBlindMode(static_cast<Settings::ColorBlindMode>(index));
            }
        );

        yPos += 55.0f;

        aer::ui::UiFactory::createText(
            _uiRegistry,
            {240.0f, yPos},
            makeText(_translation.get("settings.difficulty"), "assets/fonts/main.ttf", 22)
        );

        std::vector<std::string> difficultyOptions = {
            _translation.get("settings.easy"),
            _translation.get("settings.normal"),
            _translation.get("settings.hard"),
            _translation.get("settings.infernal")
        };

        aer::ui::UiFactory::createDropdown(
            _uiRegistry,
            {540.0f, yPos},
            {300.0f, 35.0f},
            difficultyOptions,
            static_cast<int>(_settings.difficulty()),
            [this](int index) {
                _settings.setDifficulty(static_cast<Settings::Difficulty>(index));
            }
        );

        yPos += 55.0f;

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {490.0f, yPos},
            {300.0f, 50.0f},
            makeText(_translation.get("settings.key_bindings"), "assets/fonts/main.ttf", 22),
            [this]() {
                if (_changeScene) {
                    _changeScene(SceneId::KeyBindings);
                }
            }
        );

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {810.0f, yPos},
            {60.0f, 50.0f},
            makeText("[GP]", "assets/fonts/main.ttf", 18),
            [this]() {
                if (_changeScene) {
                    _changeScene(SceneId::GamepadSettings);
                }
            }
        );

        yPos += 65.0f;

        aer::ui::UiFactory::createButton(
            _uiRegistry,
            {490.0f, yPos},
            {300.0f, 50.0f},
            makeText(_translation.get("settings.back"), "assets/fonts/main.ttf", 22),
            [this]() {
                _settings.save();
                _translation.loadLanguage(_settings.language());
                if (_changeScene) {
                    _changeScene(SceneId::Menu);
                }
            }
        );
    }

    void SettingsScene::onExit(void)
    {
        _translation.loadLanguage(_settings.language());
        _uiRegistry.clear();
        _uiRegistry.purge();
    }

    void SettingsScene::handleEvent(const aer::input::Event &event)
    {
        if (event.type == aer::input::EventType::KeyDown &&
            event.key == aer::input::KeyCode::Escape) {
            if (_changeScene) {
                _changeScene(SceneId::Menu);
            }
        }
    }

    void SettingsScene::update(float dt)
    {
        (void)dt;
        (void)_networkSystem;
    }
} // namespace rtp::client::scenes
