/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Settings implementation
*/

#include "Core/Settings.hpp"

namespace rtp::client {

    Settings::Settings() {
        initDefaultKeyBindings();
        
        if (!load("config/settings.cfg")) {
            rtp::log::warning("No settings.cfg found, using defaults from client.json");
            loadFromClientJson("config/client.json");
        } else {
            rtp::log::info("Settings loaded from config/settings.cfg");
        }
    }

    void Settings::initDefaultKeyBindings() {
        _keyBindings[KeyAction::MoveUp] = sf::Keyboard::Key::Z;
        _keyBindings[KeyAction::MoveDown] = sf::Keyboard::Key::S;
        _keyBindings[KeyAction::MoveLeft] = sf::Keyboard::Key::Q;
        _keyBindings[KeyAction::MoveRight] = sf::Keyboard::Key::D;
        _keyBindings[KeyAction::Shoot] = sf::Keyboard::Key::Space;
        _keyBindings[KeyAction::Reload] = sf::Keyboard::Key::R;
        _keyBindings[KeyAction::Pause] = sf::Keyboard::Key::Escape;
        _keyBindings[KeyAction::Menu] = sf::Keyboard::Key::Tab;
    }

    bool Settings::loadFromClientJson(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            rtp::log::error("Failed to load client.json from: {}", filename);
            return false;
        }
        
        std::string line;
        bool inAudio = false;
        bool inControls = false;
        
        while (std::getline(file, line)) {
            if (line.find("\"audio\"") != std::string::npos) {
                inAudio = true;
                inControls = false;
                continue;
            } else if (line.find("\"controls\"") != std::string::npos) {
                inControls = true;
                inAudio = false;
                continue;
            } else if (line.find("}") != std::string::npos && line.find("{") == std::string::npos) {
                inAudio = false;
                inControls = false;
            }
            
            if (inAudio) {
                if (line.find("\"masterVolume\"") != std::string::npos) {
                    size_t colonPos = line.find(':');
                    if (colonPos != std::string::npos) {
                        std::string valueStr = line.substr(colonPos + 1);
                        valueStr.erase(std::remove(valueStr.begin(), valueStr.end(), ','), valueStr.end());
                        _masterVolume = std::stof(valueStr) / 100.0f;
                    }
                } else if (line.find("\"musicVolume\"") != std::string::npos) {
                    size_t colonPos = line.find(':');
                    if (colonPos != std::string::npos) {
                        std::string valueStr = line.substr(colonPos + 1);
                        valueStr.erase(std::remove(valueStr.begin(), valueStr.end(), ','), valueStr.end());
                        _musicVolume = std::stof(valueStr) / 100.0f;
                    }
                } else if (line.find("\"sfxVolume\"") != std::string::npos) {
                    size_t colonPos = line.find(':');
                    if (colonPos != std::string::npos) {
                        std::string valueStr = line.substr(colonPos + 1);
                        valueStr.erase(std::remove(valueStr.begin(), valueStr.end(), ','), valueStr.end());
                        _sfxVolume = std::stof(valueStr) / 100.0f;
                    }
                }
            }
            
            if (inControls) {
                auto parseKeyBinding = [&](const std::string& jsonKey, KeyAction action) {
                    if (line.find(jsonKey) != std::string::npos) {
                        size_t start = line.find('"', line.find(':')) + 1;
                        size_t end = line.find('"', start);
                        if (start != std::string::npos && end != std::string::npos) {
                            std::string keyName = line.substr(start, end - start);
                            _keyBindings[action] = stringToKey(keyName);
                        }
                    }
                };
                
                parseKeyBinding("\"moveUp\"", KeyAction::MoveUp);
                parseKeyBinding("\"moveDown\"", KeyAction::MoveDown);
                parseKeyBinding("\"moveLeft\"", KeyAction::MoveLeft);
                parseKeyBinding("\"moveRight\"", KeyAction::MoveRight);
                parseKeyBinding("\"shoot\"", KeyAction::Shoot);
                parseKeyBinding("\"reload\"", KeyAction::Reload);
            }
        }
        
        rtp::log::info("Loaded default settings from client.json");
        return true;
    }

    sf::Keyboard::Key Settings::stringToKey(const std::string& keyName) const {
        static const std::unordered_map<std::string, sf::Keyboard::Key> keyMap = {
            {"A", sf::Keyboard::Key::A}, {"B", sf::Keyboard::Key::B},
            {"C", sf::Keyboard::Key::C}, {"D", sf::Keyboard::Key::D},
            {"E", sf::Keyboard::Key::E}, {"F", sf::Keyboard::Key::F},
            {"G", sf::Keyboard::Key::G}, {"H", sf::Keyboard::Key::H},
            {"I", sf::Keyboard::Key::I}, {"J", sf::Keyboard::Key::J},
            {"K", sf::Keyboard::Key::K}, {"L", sf::Keyboard::Key::L},
            {"M", sf::Keyboard::Key::M}, {"N", sf::Keyboard::Key::N},
            {"O", sf::Keyboard::Key::O}, {"P", sf::Keyboard::Key::P},
            {"Q", sf::Keyboard::Key::Q}, {"R", sf::Keyboard::Key::R},
            {"S", sf::Keyboard::Key::S}, {"T", sf::Keyboard::Key::T},
            {"U", sf::Keyboard::Key::U}, {"V", sf::Keyboard::Key::V},
            {"W", sf::Keyboard::Key::W}, {"X", sf::Keyboard::Key::X},
            {"Y", sf::Keyboard::Key::Y}, {"Z", sf::Keyboard::Key::Z},
            {"Space", sf::Keyboard::Key::Space},
            {"Escape", sf::Keyboard::Key::Escape},
            {"Tab", sf::Keyboard::Key::Tab},
            {"Up", sf::Keyboard::Key::Up},
            {"Down", sf::Keyboard::Key::Down},
            {"Left", sf::Keyboard::Key::Left},
            {"Right", sf::Keyboard::Key::Right}
        };
        
        auto it = keyMap.find(keyName);
        return (it != keyMap.end()) ? it->second : sf::Keyboard::Key::Unknown;
    }

    sf::Keyboard::Key Settings::getKey(KeyAction action) const {
        auto it = _keyBindings.find(action);
        if (it != _keyBindings.end()) {
            return it->second;
        }
        return sf::Keyboard::Key::Unknown;
    }

    void Settings::setKey(KeyAction action, sf::Keyboard::Key key) {
        _keyBindings[action] = key;
    }

    std::string Settings::getKeyName(sf::Keyboard::Key key) const {
        static const std::unordered_map<sf::Keyboard::Key, std::string> keyNames = {
            {sf::Keyboard::Key::A, "A"}, {sf::Keyboard::Key::B, "B"},
            {sf::Keyboard::Key::C, "C"}, {sf::Keyboard::Key::D, "D"},
            {sf::Keyboard::Key::E, "E"}, {sf::Keyboard::Key::F, "F"},
            {sf::Keyboard::Key::G, "G"}, {sf::Keyboard::Key::H, "H"},
            {sf::Keyboard::Key::I, "I"}, {sf::Keyboard::Key::J, "J"},
            {sf::Keyboard::Key::K, "K"}, {sf::Keyboard::Key::L, "L"},
            {sf::Keyboard::Key::M, "M"}, {sf::Keyboard::Key::N, "N"},
            {sf::Keyboard::Key::O, "O"}, {sf::Keyboard::Key::P, "P"},
            {sf::Keyboard::Key::Q, "Q"}, {sf::Keyboard::Key::R, "R"},
            {sf::Keyboard::Key::S, "S"}, {sf::Keyboard::Key::T, "T"},
            {sf::Keyboard::Key::U, "U"}, {sf::Keyboard::Key::V, "V"},
            {sf::Keyboard::Key::W, "W"}, {sf::Keyboard::Key::X, "X"},
            {sf::Keyboard::Key::Y, "Y"}, {sf::Keyboard::Key::Z, "Z"},
            {sf::Keyboard::Key::Space, "Space"},
            {sf::Keyboard::Key::Escape, "Escape"},
            {sf::Keyboard::Key::Tab, "Tab"},
            {sf::Keyboard::Key::Left, "Left"},
            {sf::Keyboard::Key::Right, "Right"},
            {sf::Keyboard::Key::Up, "Up"},
            {sf::Keyboard::Key::Down, "Down"}
        };
        
        auto it = keyNames.find(key);
        return (it != keyNames.end()) ? it->second : "Unknown";
    }

    void Settings::setMasterVolume(float volume) {
        _masterVolume = std::clamp(volume, 0.0f, 1.0f);
        
        for (auto& callback : _onMasterVolumeChanged) {
            callback(_masterVolume);
        }
    }

    void Settings::setMusicVolume(float volume) {
        _musicVolume = std::clamp(volume, 0.0f, 1.0f);
        
        for (auto& callback : _onMusicVolumeChanged) {
            callback(_musicVolume);
        }
    }

    void Settings::setSfxVolume(float volume) {
        _sfxVolume = std::clamp(volume, 0.0f, 1.0f);
        
        for (auto& callback : _onSfxVolumeChanged) {
            callback(_sfxVolume);
        }
    }

    void Settings::setLanguage(Language lang) {
        if (_language != lang) {
            _language = lang;
            
            for (auto& callback : _onLanguageChanged) {
                callback(_language);
            }
        }
    }

    std::string Settings::getLanguageName(Language lang) const {
        switch (lang) {
            case Language::English: return "English";
            case Language::French: return "Français";
            case Language::Spanish: return "Español";
            case Language::German: return "Deutsch";
            case Language::Italian: return "Italiano";
            default: return "Unknown";
        }
    }

    std::string Settings::getColorBlindModeName(ColorBlindMode mode) const {
        switch (mode) {
            case ColorBlindMode::None: return "None";
            case ColorBlindMode::Protanopia: return "Protanopia (Red-blind)";
            case ColorBlindMode::Deuteranopia: return "Deuteranopia (Green-blind)";
            case ColorBlindMode::Tritanopia: return "Tritanopia (Blue-blind)";
            default: return "Unknown";
        }
    }

    bool Settings::save(const std::string& filename) {
        std::filesystem::create_directories("config");
        
        std::ofstream file(filename);
        if (!file.is_open()) {
            rtp::log::error("Failed to save settings to: {}", filename);
            return false;
        }
        
        file << "master_volume=" << _masterVolume << "\n";
        file << "music_volume=" << _musicVolume << "\n";
        file << "sfx_volume=" << _sfxVolume << "\n";
        
        file << "language=" << static_cast<int>(_language) << "\n";
        
        file << "colorblind_mode=" << static_cast<int>(_colorBlindMode) << "\n";
        file << "high_contrast=" << (_highContrast ? 1 : 0) << "\n";
        
        for (const auto& [action, key] : _keyBindings) {
            file << "key_" << static_cast<int>(action) << "=" << static_cast<int>(key) << "\n";
        }
        
        rtp::log::info("Settings saved to: {}", filename);
        return true;
    }

    bool Settings::load(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "Failed to load settings from " << filename << ", using defaults" << std::endl;
            return false;
        }
        
        std::string line;
        while (std::getline(file, line)) {
            auto delimPos = line.find('=');
            if (delimPos == std::string::npos) continue;
            
            std::string key = line.substr(0, delimPos);
            std::string value = line.substr(delimPos + 1);
            
            try {
                if (key == "master_volume") {
                    _masterVolume = std::stof(value);
                } else if (key == "music_volume") {
                    _musicVolume = std::stof(value);
                } else if (key == "sfx_volume") {
                    _sfxVolume = std::stof(value);
                } else if (key == "language") {
                    _language = static_cast<Language>(std::stoi(value));
                } else if (key == "colorblind_mode") {
                    _colorBlindMode = static_cast<ColorBlindMode>(std::stoi(value));
                } else if (key == "high_contrast") {
                    _highContrast = (std::stoi(value) != 0);
                } else if (key.find("key_") == 0) {
                    int actionId = std::stoi(key.substr(4));
                    int keyCode = std::stoi(value);
                    _keyBindings[static_cast<KeyAction>(actionId)] = static_cast<sf::Keyboard::Key>(keyCode);
                }
            } catch (const std::exception& e) {
                std::cerr << "Error parsing setting: " << key << " = " << value << std::endl;
            }
        }
        
        return true;
    }
}  // namespace Client::Core
