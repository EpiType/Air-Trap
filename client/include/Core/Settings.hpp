/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Settings - Game
 * configuration and preferences
*/

#ifndef RTYPE_CLIENT_CORE_SETTINGS_HPP_
    #define RTYPE_CLIENT_CORE_SETTINGS_HPP_

#include "RType/Logger.hpp"
#include "RType/ECS/Components/SimpleWeapon.hpp"
#include "Utils/KeyAction.hpp"

#include <SFML/Window/Keyboard.hpp>
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace rtp::client
{
    /**
     * @enum Language
     * @brief Supported languages
     */
    enum class Language {
        English,
        French,
        Spanish,
        German,
        Italian
    };

    /**
     * @enum ColorBlindMode
     * @brief Color blind assistance modes
     */
    enum class ColorBlindMode {
        None,
        Protanopia,   // Red-blind
        Deuteranopia, // Green-blind
        Tritanopia    // Blue-blind
    };

    /**
     * @enum Difficulty
     * @brief Game difficulty levels (solo mode only)
     */
    enum class Difficulty {
        Easy,
        Normal,
        Hard,
        Infernal
    };

    /**
     * @class Settings
     * @brief Manages game settings and preferences
     */
    class Settings {
        public:
            Settings();

            sf::Keyboard::Key getKey(KeyAction action) const;
            void setKey(KeyAction action, sf::Keyboard::Key key);
            std::string getKeyName(sf::Keyboard::Key key) const;

            float getMasterVolume() const
            {
                return _masterVolume;
            }

            void setMasterVolume(float volume);

            float getMusicVolume() const
            {
                return _musicVolume;
            }

            void setMusicVolume(float volume);

            float getSfxVolume() const
            {
                return _sfxVolume;
            }

            void setSfxVolume(float volume);

            using VolumeCallback = std::function<void(float)>;

            void onMasterVolumeChanged(VolumeCallback callback)
            {
                _onMasterVolumeChanged.push_back(callback);
            }

            void onMusicVolumeChanged(VolumeCallback callback)
            {
                _onMusicVolumeChanged.push_back(callback);
            }

            void onSfxVolumeChanged(VolumeCallback callback)
            {
                _onSfxVolumeChanged.push_back(callback);
            }

            Language getLanguage() const
            {
                return _language;
            }

            void setLanguage(Language lang);
            std::string getLanguageName(Language lang) const;

            using LanguageCallback = std::function<void(Language)>;

            void onLanguageChanged(LanguageCallback callback)
            {
                _onLanguageChanged.push_back(callback);
            }

            ColorBlindMode getColorBlindMode() const
            {
                return _colorBlindMode;
            }

            void setColorBlindMode(ColorBlindMode mode)
            {
                _colorBlindMode = mode;
            }

            std::string getColorBlindModeName(ColorBlindMode mode) const;

            bool getHighContrast() const
            {
                return _highContrast;
            }

            void setHighContrast(bool enabled)
            {
                _highContrast = enabled;
            }

            Difficulty getDifficulty() const
            {
                return _difficulty;
            }

            void setDifficulty(Difficulty difficulty)
            {
                _difficulty = difficulty;
            }

            std::string getDifficultyName(Difficulty difficulty) const;

            ecs::components::WeaponKind getSelectedWeapon() const
            {
                return _selectedWeapon;
            }

            void setSelectedWeapon(ecs::components::WeaponKind weapon)
            {
                _selectedWeapon = weapon;
            }

            std::string getWeaponName(ecs::components::WeaponKind weapon) const;

            bool getGamepadEnabled() const
            {
                return _gamepadEnabled;
            }

            void setGamepadEnabled(bool enabled)
            {
                _gamepadEnabled = enabled;
            }

            float getGamepadDeadzone() const
            {
                return _gamepadDeadzone;
            }

            void setGamepadDeadzone(float deadzone)
            {
                _gamepadDeadzone = std::clamp(deadzone, 0.0f, 100.0f);
            }

            unsigned int getGamepadShootButton() const
            {
                return _gamepadShootButton;
            }

            void setGamepadShootButton(unsigned int button)
            {
                _gamepadShootButton = button;
            }

            unsigned int getGamepadReloadButton() const
            {
                return _gamepadReloadButton;
            }

            void setGamepadReloadButton(unsigned int button)
            {
                _gamepadReloadButton = button;
            }

            unsigned int getGamepadValidateButton() const
            {
                return _gamepadValidateButton;
            }

            void setGamepadValidateButton(unsigned int button)
            {
                _gamepadValidateButton = button;
            }

            float getGamepadCursorSpeed() const
            {
                return _gamepadCursorSpeed;
            }

            void setGamepadCursorSpeed(float speed)
            {
                _gamepadCursorSpeed = std::clamp(speed, 1.0f, 20.0f);
            }

            unsigned int getGamepadPauseButton() const
            {
                return _gamepadPauseButton;
            }

            void setGamepadPauseButton(unsigned int button)
            {
                _gamepadPauseButton = button;
            }

            bool save(const std::string &filename = "config/settings.cfg");
            bool load(const std::string &filename = "config/settings.cfg");

            bool loadFromClientJson(const std::string &filename);

        private:
            std::unordered_map<KeyAction, sf::Keyboard::Key> _keyBindings;
            void initDefaultKeyBindings();
            sf::Keyboard::Key stringToKey(const std::string &keyName) const;

            float _masterVolume{1.0f};
            float _musicVolume{0.7f};
            float _sfxVolume{0.8f};

            Language _language{Language::English};

            ColorBlindMode _colorBlindMode{ColorBlindMode::None};
            bool _highContrast{false};
            Difficulty _difficulty{Difficulty::Normal};
            ecs::components::WeaponKind _selectedWeapon{ecs::components::WeaponKind::Classic};

            // Gamepad settings
            bool _gamepadEnabled{true};
            float _gamepadDeadzone{15.0f}; // Percentage (0-100)
            unsigned int _gamepadShootButton{0};   // Button A (Xbox) / X (PlayStation)
            unsigned int _gamepadReloadButton{2};  // Button X (Xbox) / Square (PlayStation)
            unsigned int _gamepadValidateButton{0}; // Button A for menu navigation
            float _gamepadCursorSpeed{8.0f};       // Cursor speed for menu navigation (1-20)
            unsigned int _gamepadPauseButton{7};   // Button Start/Menu (Xbox/generic)

            std::vector<VolumeCallback> _onMasterVolumeChanged;
            std::vector<VolumeCallback> _onMusicVolumeChanged;
            std::vector<VolumeCallback> _onSfxVolumeChanged;
            std::vector<LanguageCallback> _onLanguageChanged;
    };
} // namespace rtp::client

#endif // RTYPE_CLIENT_CORE_SETTINGS_HPP_