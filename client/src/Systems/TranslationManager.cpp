/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** TranslationManager implementation
*/

#include "Systems/TranslationManager.hpp"
#include <sstream>

namespace Client::Core {

TranslationManager::TranslationManager() {
    loadLanguage(Language::English);
}

std::string TranslationManager::getLanguageFilePath(Language lang) const {
    switch (lang) {
        case Language::English: return "config/lang/en.txt";
        case Language::French: return "config/lang/fr.txt";
        case Language::Spanish: return "config/lang/es.txt";
        case Language::German: return "config/lang/de.txt";
        case Language::Italian: return "config/lang/it.txt";
        default: return "config/lang/en.txt";
    }
}

bool TranslationManager::loadLanguage(Language language) {
    _translations.clear();
    _currentLanguage = language;
    
    std::string filePath = getLanguageFilePath(language);
    std::ifstream file(filePath);
    
    if (!file.is_open()) {
        rtp::log::error("Failed to load language file: {}", filePath);
        return false;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Skip empty lines and comments
        if (line.empty() || line[0] == '#') continue;
        
        // Parse key=value format
        auto delimPos = line.find('=');
        if (delimPos == std::string::npos) continue;
        
        std::string key = line.substr(0, delimPos);
        std::string value = line.substr(delimPos + 1);
        
        // Remove leading/trailing whitespace
        key.erase(0, key.find_first_not_of(" \t"));
        key.erase(key.find_last_not_of(" \t") + 1);
        value.erase(0, value.find_first_not_of(" \t"));
        value.erase(value.find_last_not_of(" \t") + 1);
        
        _translations[key] = value;
    }
    
    rtp::log::info("Loaded {} translations for language: {}", 
                   _translations.size(), static_cast<int>(language));
    return true;
}

std::string TranslationManager::get(const std::string& key) const {
    auto it = _translations.find(key);
    if (it != _translations.end()) {
        return it->second;
    }
    
    rtp::log::warning("Translation key not found: {}", key);
    return key; // Return key as fallback
}

}  // namespace Client::Core