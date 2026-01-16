/**
 * File   : TranslationManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#include "rtype/client/TranslationManager.hpp"

#include <fstream>
#include <sstream>

namespace rtp::client
{
    bool TranslationManager::loadLanguage(Settings::Language language)
    {
        const std::string path = getLanguageFilePath(language);
        std::ifstream file(path);
        if (!file.is_open()) {
            return false;
        }

        _translations.clear();
        std::string line;
        while (std::getline(file, line)) {
            line = trim(line);
            if (line.empty() || line[0] == '#') {
                continue;
            }

            const auto eq = line.find('=');
            if (eq == std::string::npos) {
                continue;
            }

            const std::string key = trim(line.substr(0, eq));
            const std::string value = trim(line.substr(eq + 1));
            if (!key.empty()) {
                _translations[key] = value;
            }
        }

        _currentLanguage = language;
        return true;
    }

    std::string TranslationManager::get(const std::string& key) const
    {
        const auto it = _translations.find(key);
        if (it != _translations.end()) {
            return it->second;
        }
        return key;
    }

    Settings::Language TranslationManager::currentLanguage() const
    {
        return _currentLanguage;
    }

    std::string TranslationManager::getLanguageFilePath(Settings::Language lang) const
    {
        switch (lang) {
            case Settings::Language::English:
                return "config/lang/en.txt";
            case Settings::Language::French:
                return "config/lang/fr.txt";
            case Settings::Language::Spanish:
                return "config/lang/es.txt";
            case Settings::Language::German:
                return "config/lang/de.txt";
            case Settings::Language::Italian:
                return "config/lang/it.txt";
        }
        return "config/lang/en.txt";
    }

    std::string TranslationManager::trim(const std::string& value)
    {
        const auto first = value.find_first_not_of(" \t\r\n");
        if (first == std::string::npos) {
            return {};
        }
        const auto last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, last - first + 1);
    }
} // namespace rtp::client
