/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Settings - Game configuration and preferences
*/

#pragma once

#include "RType/Logger.hpp"
#include <algorithm>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <SFML/Window/Keyboard.hpp>
#include <string>
#include <unordered_map>
#include <fstream>
#include <functional>
#include <vector>

namespace Client::Core {

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
    Protanopia,    // Red-blind
    Deuteranopia,  // Green-blind
    Tritanopia     // Blue-blind
};

/**
 * @enum KeyAction
 * @brief Actions that can be bound to keys
 */
enum class KeyAction {
    MoveUp,
    MoveDown,
    MoveLeft,
    MoveRight,
    Shoot,
    Pause,
    Menu
};

/**
 * @class Settings
 * @brief Manages game settings and preferences
 */
class Settings {
public:
    Settings();
    
    // Key bindings
    sf::Keyboard::Key getKey(KeyAction action) const;
    void setKey(KeyAction action, sf::Keyboard::Key key);
    std::string getKeyName(sf::Keyboard::Key key) const;
    
    // Audio
    float getMasterVolume() const { return _masterVolume; }
    void setMasterVolume(float volume);
    float getMusicVolume() const { return _musicVolume; }
    void setMusicVolume(float volume);
    float getSfxVolume() const { return _sfxVolume; }
    void setSfxVolume(float volume);
    
    using VolumeCallback = std::function<void(float)>;
    void onMasterVolumeChanged(VolumeCallback callback) {
        _onMasterVolumeChanged.push_back(callback);
    }
    void onMusicVolumeChanged(VolumeCallback callback) {
        _onMusicVolumeChanged.push_back(callback);
    }
    void onSfxVolumeChanged(VolumeCallback callback) {
        _onSfxVolumeChanged.push_back(callback);
    }
    
    // Language
    Language getLanguage() const { return _language; }
    void setLanguage(Language lang);
    std::string getLanguageName(Language lang) const;
    
    using LanguageCallback = std::function<void(Language)>;
    void onLanguageChanged(LanguageCallback callback) {
        _onLanguageChanged.push_back(callback);
    }
    
    // Accessibility
    ColorBlindMode getColorBlindMode() const { return _colorBlindMode; }
    void setColorBlindMode(ColorBlindMode mode) { _colorBlindMode = mode; }
    std::string getColorBlindModeName(ColorBlindMode mode) const;
    
    bool getHighContrast() const { return _highContrast; }
    void setHighContrast(bool enabled) { _highContrast = enabled; }
    
    // Persistence
    bool save(const std::string& filename = "config/settings.cfg");
    bool load(const std::string& filename = "config/settings.cfg");

    bool loadFromClientJson(const std::string &filename);
    
private:
    // Key bindings
    std::unordered_map<KeyAction, sf::Keyboard::Key> _keyBindings;
    void initDefaultKeyBindings();
    sf::Keyboard::Key stringToKey(const std::string &keyName) const;
    
    // Audio settings
    float _masterVolume{1.0f};
    float _musicVolume{0.7f};
    float _sfxVolume{0.8f};
    
    // Language
    Language _language{Language::English};
    
    // Accessibility
    ColorBlindMode _colorBlindMode{ColorBlindMode::None};
    bool _highContrast{false};
    
    std::vector<VolumeCallback> _onMasterVolumeChanged;
    std::vector<VolumeCallback> _onMusicVolumeChanged;
    std::vector<VolumeCallback> _onSfxVolumeChanged;
    std::vector<LanguageCallback> _onLanguageChanged;
};

}  // namespace Client::Core
