/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** TranslationManager - Handles game translations
*/

#ifndef TRANSLATIONMANAGER_HPP_
#define TRANSLATIONMANAGER_HPP_

#include <string>
#include <unordered_map>
#include <fstream>
#include "Core/Settings.hpp"
#include "RType/Logger.hpp"

namespace Client::Core {

/**
 * @class TranslationManager
 * @brief Manages game translations for all UI elements
 */
class TranslationManager {
public:
    TranslationManager() = default;
    
    /**
     * @brief Load translations for a specific language
     * @param language The language to load
     * @return true if successful, false otherwise
     */
    bool loadLanguage(Language language);
    
    /**
     * @brief Get translated string for a key
     * @param key The translation key (e.g., "menu.play")
     * @return Translated string, or the key itself if not found
     */
    std::string get(const std::string& key) const;
    
    /**
     * @brief Get translated string with format arguments
     * @tparam Args Format argument types
     * @param key The translation key
     * @param args Format arguments
     * @return Formatted translated string
     */
    template<typename... Args>
    std::string getf(const std::string& key, Args&&... args) const {
        std::string format = get(key);
        // Simple replacement for now, you can use std::format in C++23
        return format;
    }
    
    Language getCurrentLanguage() const { return _currentLanguage; }

private:
    std::unordered_map<std::string, std::string> _translations;
    Language _currentLanguage{Language::English};
    
    std::string getLanguageFilePath(Language lang) const;
};

}  // namespace Client::Core

#endif /* !TRANSLATIONMANAGER_HPP_ */