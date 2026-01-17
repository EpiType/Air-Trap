/**
 * File   : TranslationManager.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_TRANSLATION_MANAGER_HPP_
    #define RTYPE_CLIENT_TRANSLATION_MANAGER_HPP_

    #include "rtype/utility/Settings.hpp"

    #include <string>
    #include <unordered_map>

namespace rtp::client
{
    class TranslationManager
    {
        public:
            TranslationManager() = default;

            bool loadLanguage(Settings::Language language);
            std::string get(const std::string& key) const;

            Settings::Language currentLanguage() const;

        private:
            std::string getLanguageFilePath(Settings::Language lang) const;
            static std::string trim(const std::string& value);

            std::unordered_map<std::string, std::string> _translations;
            Settings::Language _currentLanguage{Settings::Language::English};
    };
} // namespace rtp::client

#endif // RTYPE_CLIENT_TRANSLATION_MANAGER_HPP_
