/**
 * File   : Settings.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_SETTINGS_HPP_
    #define RTYPE_CLIENT_SETTINGS_HPP_

    #include <algorithm>
    #include <cstdint>
    #include <string>

namespace rtp::client
{
    class Settings
    {
        public:
            enum class Language : std::uint8_t {
                English,
                French,
                Spanish,
                German,
                Italian
            };

            enum class ColorBlindMode : std::uint8_t {
                None,
                Protanopia,
                Deuteranopia,
                Tritanopia
            };

            enum class Difficulty : std::uint8_t {
                Easy,
                Normal,
                Hard,
                Infernal
            };

            Settings() = default;

            float masterVolume() const;
            void setMasterVolume(float volume);

            float musicVolume() const;
            void setMusicVolume(float volume);

            float sfxVolume() const;
            void setSfxVolume(float volume);

            Language language() const;
            void setLanguage(Language value);
            static std::string languageName(Language value);

            ColorBlindMode colorBlindMode() const;
            void setColorBlindMode(ColorBlindMode value);
            static std::string colorBlindModeName(ColorBlindMode value);

            bool highContrast() const;
            void setHighContrast(bool enabled);

            Difficulty difficulty() const;
            void setDifficulty(Difficulty value);
            static std::string difficultyName(Difficulty value);

            bool gamepadEnabled() const;
            void setGamepadEnabled(bool enabled);

            float gamepadDeadzone() const;
            void setGamepadDeadzone(float value);

            unsigned int gamepadShootButton() const;
            void setGamepadShootButton(unsigned int button);

            unsigned int gamepadReloadButton() const;
            void setGamepadReloadButton(unsigned int button);

            unsigned int gamepadValidateButton() const;
            void setGamepadValidateButton(unsigned int button);

            float gamepadCursorSpeed() const;
            void setGamepadCursorSpeed(float speed);

            unsigned int gamepadPauseButton() const;
            void setGamepadPauseButton(unsigned int button);

            bool save(const std::string &filename = "config/settings.cfg") const;
            bool load(const std::string &filename = "config/settings.cfg");

        private:
            static float clampVolume(float value);
            static std::string trim(const std::string &value);
            static Language parseLanguage(const std::string &value);
            static ColorBlindMode parseColorBlindMode(const std::string &value);
            static Difficulty parseDifficulty(const std::string &value);

            float _masterVolume{1.0f};
            float _musicVolume{0.7f};
            float _sfxVolume{0.8f};

            Language _language{Language::English};
            ColorBlindMode _colorBlindMode{ColorBlindMode::None};
            bool _highContrast{false};
            Difficulty _difficulty{Difficulty::Normal};

            bool _gamepadEnabled{true};
            float _gamepadDeadzone{15.0f};
            unsigned int _gamepadShootButton{0};
            unsigned int _gamepadReloadButton{2};
            unsigned int _gamepadValidateButton{0};
            float _gamepadCursorSpeed{8.0f};
            unsigned int _gamepadPauseButton{7};
    };
} // namespace rtp::client

#endif // RTYPE_CLIENT_SETTINGS_HPP_
