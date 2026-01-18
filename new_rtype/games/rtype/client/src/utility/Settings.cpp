/**
 * File   : Settings.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/utility/Settings.hpp"

#include <cctype>
#include <fstream>
#include <sstream>

namespace rtp::client
{
    Settings::Settings()
    {
        _keyBindings[actionIndex(KeyAction::MoveUp)] = aer::input::KeyCode::W;
        _keyBindings[actionIndex(KeyAction::MoveDown)] = aer::input::KeyCode::S;
        _keyBindings[actionIndex(KeyAction::MoveLeft)] = aer::input::KeyCode::A;
        _keyBindings[actionIndex(KeyAction::MoveRight)] = aer::input::KeyCode::D;
        _keyBindings[actionIndex(KeyAction::Shoot)] = aer::input::KeyCode::Space;
        _keyBindings[actionIndex(KeyAction::Reload)] = aer::input::KeyCode::R;
        _keyBindings[actionIndex(KeyAction::Pause)] = aer::input::KeyCode::Escape;

        load();
    }

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

    aer::input::KeyCode Settings::getKey(KeyAction action) const
    {
        return _keyBindings[actionIndex(action)];
    }

    void Settings::setKey(KeyAction action, aer::input::KeyCode key)
    {
        _keyBindings[actionIndex(action)] = key;
    }

    std::string Settings::getKeyName(aer::input::KeyCode key) const
    {
        return keyToString(key);
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
        file << "key_move_up=" << keyToString(getKey(KeyAction::MoveUp)) << "\n";
        file << "key_move_down=" << keyToString(getKey(KeyAction::MoveDown)) << "\n";
        file << "key_move_left=" << keyToString(getKey(KeyAction::MoveLeft)) << "\n";
        file << "key_move_right=" << keyToString(getKey(KeyAction::MoveRight)) << "\n";
        file << "key_shoot=" << keyToString(getKey(KeyAction::Shoot)) << "\n";
        file << "key_reload=" << keyToString(getKey(KeyAction::Reload)) << "\n";
        file << "key_pause=" << keyToString(getKey(KeyAction::Pause)) << "\n";

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
            } else if (key == "key_move_up") {
                setKey(KeyAction::MoveUp, stringToKey(value));
            } else if (key == "key_move_down") {
                setKey(KeyAction::MoveDown, stringToKey(value));
            } else if (key == "key_move_left") {
                setKey(KeyAction::MoveLeft, stringToKey(value));
            } else if (key == "key_move_right") {
                setKey(KeyAction::MoveRight, stringToKey(value));
            } else if (key == "key_shoot") {
                setKey(KeyAction::Shoot, stringToKey(value));
            } else if (key == "key_reload") {
                setKey(KeyAction::Reload, stringToKey(value));
            } else if (key == "key_pause") {
                setKey(KeyAction::Pause, stringToKey(value));
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

    std::string Settings::keyToString(aer::input::KeyCode key)
    {
        using aer::input::KeyCode;
        switch (key) {
            case KeyCode::Up: return "Up";
            case KeyCode::Down: return "Down";
            case KeyCode::Left: return "Left";
            case KeyCode::Right: return "Right";
            case KeyCode::Escape: return "Escape";
            case KeyCode::Return: return "Enter";
            case KeyCode::Space: return "Space";
            case KeyCode::F1: return "F1";
            case KeyCode::A: return "A";
            case KeyCode::B: return "B";
            case KeyCode::C: return "C";
            case KeyCode::D: return "D";
            case KeyCode::E: return "E";
            case KeyCode::F: return "F";
            case KeyCode::G: return "G";
            case KeyCode::H: return "H";
            case KeyCode::I: return "I";
            case KeyCode::J: return "J";
            case KeyCode::K: return "K";
            case KeyCode::L: return "L";
            case KeyCode::M: return "M";
            case KeyCode::N: return "N";
            case KeyCode::O: return "O";
            case KeyCode::P: return "P";
            case KeyCode::Q: return "Q";
            case KeyCode::R: return "R";
            case KeyCode::S: return "S";
            case KeyCode::T: return "T";
            case KeyCode::U: return "U";
            case KeyCode::V: return "V";
            case KeyCode::W: return "W";
            case KeyCode::X: return "X";
            case KeyCode::Y: return "Y";
            case KeyCode::Z: return "Z";
            case KeyCode::Num0: return "0";
            case KeyCode::Num1: return "1";
            case KeyCode::Num2: return "2";
            case KeyCode::Num3: return "3";
            case KeyCode::Num4: return "4";
            case KeyCode::Num5: return "5";
            case KeyCode::Num6: return "6";
            case KeyCode::Num7: return "7";
            case KeyCode::Num8: return "8";
            case KeyCode::Num9: return "9";
            default: return "Unknown";
        }
    }

    aer::input::KeyCode Settings::stringToKey(const std::string &value)
    {
        std::string upper;
        upper.reserve(value.size());
        for (char c : value) {
            upper.push_back(static_cast<char>(std::toupper(static_cast<unsigned char>(c))));
        }

        if (upper == "UP") return aer::input::KeyCode::Up;
        if (upper == "DOWN") return aer::input::KeyCode::Down;
        if (upper == "LEFT") return aer::input::KeyCode::Left;
        if (upper == "RIGHT") return aer::input::KeyCode::Right;
        if (upper == "ESC" || upper == "ESCAPE") return aer::input::KeyCode::Escape;
        if (upper == "ENTER" || upper == "RETURN") return aer::input::KeyCode::Return;
        if (upper == "SPACE") return aer::input::KeyCode::Space;
        if (upper == "F1") return aer::input::KeyCode::F1;
        if (upper == "0") return aer::input::KeyCode::Num0;
        if (upper == "1") return aer::input::KeyCode::Num1;
        if (upper == "2") return aer::input::KeyCode::Num2;
        if (upper == "3") return aer::input::KeyCode::Num3;
        if (upper == "4") return aer::input::KeyCode::Num4;
        if (upper == "5") return aer::input::KeyCode::Num5;
        if (upper == "6") return aer::input::KeyCode::Num6;
        if (upper == "7") return aer::input::KeyCode::Num7;
        if (upper == "8") return aer::input::KeyCode::Num8;
        if (upper == "9") return aer::input::KeyCode::Num9;

        if (upper.size() == 1 && upper[0] >= 'A' && upper[0] <= 'Z') {
            return static_cast<aer::input::KeyCode>(
                static_cast<int>(aer::input::KeyCode::A) + (upper[0] - 'A'));
        }

        return aer::input::KeyCode::Unknown;
    }

    std::size_t Settings::actionIndex(KeyAction action)
    {
        return static_cast<std::size_t>(action);
    }
} // namespace rtp::client
