/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Settings implementation
*/

#include "Core/Settings.hpp"
#include <sstream>
#include <iostream>

namespace Client::Core {

Settings::Settings() {
    initDefaultKeyBindings();
}

void Settings::initDefaultKeyBindings() {
    _keyBindings[KeyAction::MoveUp] = sf::Keyboard::Key::Z;
    _keyBindings[KeyAction::MoveDown] = sf::Keyboard::Key::S;
    _keyBindings[KeyAction::MoveLeft] = sf::Keyboard::Key::Q;
    _keyBindings[KeyAction::MoveRight] = sf::Keyboard::Key::D;
    _keyBindings[KeyAction::Shoot] = sf::Keyboard::Key::Space;
    _keyBindings[KeyAction::Pause] = sf::Keyboard::Key::Escape;
    _keyBindings[KeyAction::Menu] = sf::Keyboard::Key::Tab;
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
    // Mapping des touches SFML vers noms lisibles
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
}

void Settings::setMusicVolume(float volume) {
    _musicVolume = std::clamp(volume, 0.0f, 1.0f);
}

void Settings::setSfxVolume(float volume) {
    _sfxVolume = std::clamp(volume, 0.0f, 1.0f);
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
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to save settings to " << filename << std::endl;
        return false;
    }
    
    // Audio
    file << "master_volume=" << _masterVolume << "\n";
    file << "music_volume=" << _musicVolume << "\n";
    file << "sfx_volume=" << _sfxVolume << "\n";
    
    // Language
    file << "language=" << static_cast<int>(_language) << "\n";
    
    // Accessibility
    file << "colorblind_mode=" << static_cast<int>(_colorBlindMode) << "\n";
    file << "high_contrast=" << (_highContrast ? 1 : 0) << "\n";
    
    // Key bindings
    for (const auto& [action, key] : _keyBindings) {
        file << "key_" << static_cast<int>(action) << "=" << static_cast<int>(key) << "\n";
    }
    
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
