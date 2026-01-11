/**
 * File   : SettingsScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/SettingsScene.hpp"
#include <vector>
#include <string>
#include <algorithm>

namespace rtp::client {
    namespace Scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        SettingsScene::SettingsScene(ecs::Registry& UiRegistry,
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

        void SettingsScene::onEnter(void)
        {
            rtp::log::info("Entering SettingsScene");
            float yPos = 150.0f;

            _uiFactory.createText(
                _uiRegistry,
                {500.0f, 50.0f},
                _translationManager.get("settings.title"),
                "assets/fonts/main.ttf",
                60,
                10,
                {255, 200, 100}
            );

            _uiFactory.createText(
                _uiRegistry,
                {200.0f, yPos},
                _translationManager.get("settings.master_volume"),
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createSlider(
                _uiRegistry,
                {450.0f, yPos + 5.0f},
                {300.0f, 15.0f},
                0.0f,
                1.0f,
                _settings.getMasterVolume(),
                [this](float value) {
                    _settings.setMasterVolume(value);
                }
            );

            yPos += 50.0f;

            _uiFactory.createText(
                _uiRegistry,
                {200.0f, yPos},
                _translationManager.get("settings.music_volume"),
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createSlider(
                _uiRegistry,
                {450.0f, yPos + 5.0f},
                {300.0f, 15.0f},
                0.0f,
                1.0f,
                _settings.getMusicVolume(),
                [this](float value) {
                    _settings.setMusicVolume(value);
                }
            );

            yPos += 50.0f;

            _uiFactory.createText(
                _uiRegistry,
                {200.0f, yPos},
                _translationManager.get("settings.sfx_volume"),
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createSlider(
                _uiRegistry,
                {450.0f, yPos + 5.0f},
                {300.0f, 15.0f},
                0.0f,
                1.0f,
                _settings.getSfxVolume(),
                [this](float value) {
                    _settings.setSfxVolume(value);
                }
            );

            yPos += 80.0f;

            _uiFactory.createText(
                _uiRegistry,
                {200.0f, yPos},
                _translationManager.get("settings.language"),
                "assets/fonts/main.ttf",
                24
            );

            _uiFactory.createDropdown(
                _uiRegistry,
                {450.0f, yPos - 5.0f},
                {300.0f, 35.0f},
                std::vector<std::string>{"English", "Français", "Español", "Deutsch", "Italiano"},
                static_cast<int>(_settings.getLanguage()),
                [this](int index) {
                    _settings.setLanguage(
                        static_cast<Language>(index));
                    _translationManager.loadLanguage(_settings.getLanguage());
                    _changeState(GameState::Settings);
                }
            );

            yPos += 80.0f;

            _uiFactory.createText(
                _uiRegistry,
                {200.0f, yPos},
                _translationManager.get("settings.colorblind_mode"),
                "assets/fonts/main.ttf",
                24
            );

            std::vector<std::string> colorblindOptions = {
                _translationManager.get("colorblind.none"),
                _translationManager.get("colorblind.protanopia"),
                _translationManager.get("colorblind.deuteranopia"),
                _translationManager.get("colorblind.tritanopia")
            };

            _uiFactory.createDropdown(
                _uiRegistry,
                {450.0f, yPos - 5.0f},
                {300.0f, 35.0f},
                colorblindOptions,
                static_cast<int>(_settings.getColorBlindMode()),
                [this, colorblindOptions](int index) {
                    _settings.setColorBlindMode(
                        static_cast<ColorBlindMode>(index));
                }
            );

            yPos += 80.0f;

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, yPos},
                {300.0f, 60.0f},
                _translationManager.get("settings.key_bindings"),
                [this]() {
                    _changeState(GameState::KeyBindings);
                }
            );

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 600.0f},
                {300.0f, 60.0f},
                _translationManager.get("settings.back"),
                [this]() {
                    _settings.save();
                    _changeState(GameState::Menu);
                }
            );
        }

        void SettingsScene::onExit(void)
        {
            _uiRegistry.clear();
        }

        void SettingsScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void SettingsScene::update(float dt)
        {
            (void)dt;
        }
    } // namespace Scenes
} // namespace rtp::client