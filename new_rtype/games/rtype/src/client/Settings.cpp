/**
 * File   : Settings.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/client/Settings.hpp"

#include <fstream>
#include <sstream>

namespace rtp::client
{
    float Settings::masterVolume() const
    {
        return _masterVolume;
    }

    void Settings::setMasterVolume(float volume)
    {
        _masterVolume = clampVolume(volume);
    }

    float Settings::musicVolume() const
    {
        return _musicVolume;
    }

    void Settings::setMusicVolume(float volume)
    {
        _musicVolume = clampVolume(volume);
    }

    float Settings::sfxVolume() const
    {
        return _sfxVolume;
    }

    void Settings::setSfxVolume(float volume)
    {
        _sfxVolume = clampVolume(volume);
    }

    Settings::Language Settings::language() const
    {
        return _language;
    }

    void Settings::setLanguage(Language value)
    {
        _language = value;
    }

    std::string Settings::languageName(Language value)
    {
        switch (value) {
            case Language::English:
                return "English";
            case Language::French:
                return "French";
            case Language::Spanish:
                return "Spanish";
            case Language::German:
                return "German";
            case Language::Italian:
                return "Italian";
        }
        return "English";
    }

    Settings::ColorBlindMode Settings::colorBlindMode() const
    {
        return _colorBlindMode;
    }

    void Settings::setColorBlindMode(ColorBlindMode value)
    {
        _colorBlindMode = value;
    }

    std::string Settings::colorBlindModeName(ColorBlindMode value)
    {
        switch (value) {
            case ColorBlindMode::None:
                return "None";
            case ColorBlindMode::Protanopia:
                return "Protanopia";
            case ColorBlindMode::Deuteranopia:
                return "Deuteranopia";
            case ColorBlindMode::Tritanopia:
                return "Tritanopia";
        }
        return "None";
    }

    bool Settings::highContrast() const
    {
        return _highContrast;
    }

    void Settings::setHighContrast(bool enabled)
    {
        _highContrast = enabled;
    }

    Settings::Difficulty Settings::difficulty() const
    {
        return _difficulty;
    }

    void Settings::setDifficulty(Difficulty value)
    {
        _difficulty = value;
    }

    std::string Settings::difficultyName(Difficulty value)
    {
        switch (value) {
            case Difficulty::Easy:
                return "Easy";
            case Difficulty::Normal:
                return "Normal";
            case Difficulty::Hard:
                return "Hard";
            case Difficulty::Infernal:
                return "Infernal";
        }
        return "Normal";
    }

    bool Settings::gamepadEnabled() const
    {
        return _gamepadEnabled;
    }

    void Settings::setGamepadEnabled(bool enabled)
    {
        _gamepadEnabled = enabled;
    }

    float Settings::gamepadDeadzone() const
    {
        return _gamepadDeadzone;
    }

    void Settings::setGamepadDeadzone(float value)
    {
        _gamepadDeadzone = std::clamp(value, 0.0f, 100.0f);
    }

    unsigned int Settings::gamepadShootButton() const
    {
        return _gamepadShootButton;
    }

    void Settings::setGamepadShootButton(unsigned int button)
    {
        _gamepadShootButton = button;
    }

    unsigned int Settings::gamepadReloadButton() const
    {
        return _gamepadReloadButton;
    }

    void Settings::setGamepadReloadButton(unsigned int button)
    {
        _gamepadReloadButton = button;
    }

    unsigned int Settings::gamepadValidateButton() const
    {
        return _gamepadValidateButton;
    }

    void Settings::setGamepadValidateButton(unsigned int button)
    {
        _gamepadValidateButton = button;
    }

    float Settings::gamepadCursorSpeed() const
    {
        return _gamepadCursorSpeed;
    }

    void Settings::setGamepadCursorSpeed(float speed)
    {
        _gamepadCursorSpeed = std::clamp(speed, 1.0f, 20.0f);
    }

    unsigned int Settings::gamepadPauseButton() const
    {
        return _gamepadPauseButton;
    }

    void Settings::setGamepadPauseButton(unsigned int button)
    {
        _gamepadPauseButton = button;
    }

    bool Settings::save(const std::string &filename) const
    {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        file << "master_volume=" << _masterVolume << "\n";
        file << "music_volume=" << _musicVolume << "\n";
        file << "sfx_volume=" << _sfxVolume << "\n";
        file << "language=" << languageName(_language) << "\n";
        file << "color_blind_mode=" << colorBlindModeName(_colorBlindMode) << "\n";
        file << "high_contrast=" << (_highContrast ? "1" : "0") << "\n";
        file << "difficulty=" << difficultyName(_difficulty) << "\n";
        file << "gamepad_enabled=" << (_gamepadEnabled ? "1" : "0") << "\n";
        file << "gamepad_deadzone=" << _gamepadDeadzone << "\n";
        file << "gamepad_shoot_button=" << _gamepadShootButton << "\n";
        file << "gamepad_reload_button=" << _gamepadReloadButton << "\n";
        file << "gamepad_validate_button=" << _gamepadValidateButton << "\n";
        file << "gamepad_cursor_speed=" << _gamepadCursorSpeed << "\n";
        file << "gamepad_pause_button=" << _gamepadPauseButton << "\n";

        return true;
    }

    bool Settings::load(const std::string &filename)
    {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            if (line.empty() || line[0] == '#') {
                continue;
            }

            const auto eq = line.find('=');
            if (eq == std::string::npos) {
                continue;
            }

            const std::string key = trim(line.substr(0, eq));
            const std::string value = trim(line.substr(eq + 1));

            if (key == "master_volume") {
                setMasterVolume(std::stof(value));
            } else if (key == "music_volume") {
                setMusicVolume(std::stof(value));
            } else if (key == "sfx_volume") {
                setSfxVolume(std::stof(value));
            } else if (key == "language") {
                _language = parseLanguage(value);
            } else if (key == "color_blind_mode") {
                _colorBlindMode = parseColorBlindMode(value);
            } else if (key == "high_contrast") {
                _highContrast = (value == "1" || value == "true");
            } else if (key == "difficulty") {
                _difficulty = parseDifficulty(value);
            } else if (key == "gamepad_enabled") {
                _gamepadEnabled = (value == "1" || value == "true");
            } else if (key == "gamepad_deadzone") {
                setGamepadDeadzone(std::stof(value));
            } else if (key == "gamepad_shoot_button") {
                _gamepadShootButton = static_cast<unsigned int>(std::stoul(value));
            } else if (key == "gamepad_reload_button") {
                _gamepadReloadButton = static_cast<unsigned int>(std::stoul(value));
            } else if (key == "gamepad_validate_button") {
                _gamepadValidateButton = static_cast<unsigned int>(std::stoul(value));
            } else if (key == "gamepad_cursor_speed") {
                setGamepadCursorSpeed(std::stof(value));
            } else if (key == "gamepad_pause_button") {
                _gamepadPauseButton = static_cast<unsigned int>(std::stoul(value));
            }
        }

        return true;
    }

    float Settings::clampVolume(float value)
    {
        return std::clamp(value, 0.0f, 1.0f);
    }

    std::string Settings::trim(const std::string &value)
    {
        const auto first = value.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return {};
        }
        const auto last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, last - first + 1);
    }

    Settings::Language Settings::parseLanguage(const std::string &value)
    {
        if (value == "French") {
            return Language::French;
        }
        if (value == "Spanish") {
            return Language::Spanish;
        }
        if (value == "German") {
            return Language::German;
        }
        if (value == "Italian") {
            return Language::Italian;
        }
        return Language::English;
    }

    Settings::ColorBlindMode Settings::parseColorBlindMode(const std::string &value)
    {
        if (value == "Protanopia") {
            return ColorBlindMode::Protanopia;
        }
        if (value == "Deuteranopia") {
            return ColorBlindMode::Deuteranopia;
        }
        if (value == "Tritanopia") {
            return ColorBlindMode::Tritanopia;
        }
        return ColorBlindMode::None;
    }

    Settings::Difficulty Settings::parseDifficulty(const std::string &value)
    {
        if (value == "Easy") {
            return Difficulty::Easy;
        }
        if (value == "Hard") {
            return Difficulty::Hard;
        }
        if (value == "Infernal") {
            return Difficulty::Infernal;
        }
        return Difficulty::Normal;
    }
} // namespace rtp::client
