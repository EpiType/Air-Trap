/**
 * File   : MenuScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/MenuScene.hpp"
#include "RType/ECS/Components/Audio/AudioSource.hpp"
#include <vector>
#include "RType/Config/WeaponConfig.hpp"

namespace rtp::client {
    namespace scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        MenuScene::MenuScene(ecs::Registry& UiRegistry,
                             Settings& settings,
                             TranslationManager& translationManager,
                             NetworkSyncSystem& network,
                             graphics::UiFactory& uiFactory,
                             std::function<void(GameState)> changeState)
            : _uiRegistry(UiRegistry),
              _settings(settings),
              _translationManager(translationManager),
              _network(network),
              _uiFactory(uiFactory),
              _changeState(changeState),
              _menuMusicEntity()
        {
        }

        void MenuScene::onEnter(void)
        {
            log::info("Entering MenuScene");

            // Ensure we load the latest weapon configurations so UI shows correct details
            rtp::config::reloadWeaponConfigs();

            auto audioSources = _uiRegistry.get<ecs::components::audio::AudioSource>();
            bool musicAlreadyPlaying = false;
            
            if (audioSources) {
                auto& sources = audioSources.value().get();
                for (const auto& entity : sources.entities()) {
                    auto& source = sources[entity];
                    if (source.audioPath == "assets/musics/menu.mp3") {
                        musicAlreadyPlaying = true;
                        _menuMusicEntity = entity;
                        log::info("Menu music already playing, reusing it");
                        break;
                    }
                }
            }

            if (!musicAlreadyPlaying) {
                auto musicEntity = _uiRegistry.spawn();
                if (musicEntity) {
                    _menuMusicEntity = musicEntity.value();
                    ecs::components::audio::AudioSource menuMusic;
                    menuMusic.audioPath = "assets/musics/menu.mp3";
                    menuMusic.volume = 80.0f;
                    menuMusic.loop = true;
                    menuMusic.isPlaying = true;
                    menuMusic.dirty = true;
                    
                    _uiRegistry.add<ecs::components::audio::AudioSource>(_menuMusicEntity, menuMusic);
                    log::info("Playing menu music");
                }
            }

            _uiFactory.createText(
                _uiRegistry,
                {400.0f, 100.0f},
                _translationManager.get("menu.title"),
                "assets/fonts/title.ttf",
                72,
                10,
                {2, 100, 100}
            );

            // Weapon selector UI (left side)
            _uiFactory.createText(
                _uiRegistry,
                {100.0f, 240.0f},
                "WEAPON:",
                "assets/fonts/main.ttf",
                24,
                5,
                {255, 255, 255}
            );

            // Left arrow button
            _uiFactory.createButton(
                _uiRegistry,
                {80.0f, 280.0f},
                {50.0f, 50.0f},
                "<",
                [this]() {
                    auto current = static_cast<int>(_settings.getSelectedWeapon());
                    current = (current == 0) ? 4 : current - 1;
                    _settings.setSelectedWeapon(static_cast<ecs::components::WeaponKind>(current));
                    _settings.save("config/settings.cfg");
                    updateWeaponDisplay();
                    _network.sendSelectedWeapon(static_cast<uint8_t>(_settings.getSelectedWeapon()));
                }
            );

            // Weapon name text (initially created, will be updated)
            // Prefer display name from weapon configs when available
            std::string initialWeaponName = _settings.getWeaponName(_settings.getSelectedWeapon());
            if (rtp::config::hasWeaponConfigs()) {
                auto dn = rtp::config::getWeaponDisplayName(_settings.getSelectedWeapon());
                if (!dn.empty()) initialWeaponName = dn;
            }

            _weaponNameText = _uiFactory.createText(
                _uiRegistry,
                {150.0f, 290.0f},
                initialWeaponName,
                "assets/fonts/main.ttf",
                20,
                5,
                {255, 255, 0}
            );

            // Right arrow button
            _uiFactory.createButton(
                _uiRegistry,
                {330.0f, 280.0f},
                {50.0f, 50.0f},
                ">",
                [this]() {
                    auto current = static_cast<int>(_settings.getSelectedWeapon());
                    current = (current + 1) % 5;
                    _settings.setSelectedWeapon(static_cast<ecs::components::WeaponKind>(current));
                    _settings.save("config/settings.cfg");
                    updateWeaponDisplay();
                    _network.sendSelectedWeapon(static_cast<uint8_t>(_settings.getSelectedWeapon()));
                }
            );

            // Weapon stats panel
            _weaponStatsText = _uiFactory.createText(
                _uiRegistry,
                {100.0f, 350.0f},
                "",
                "assets/fonts/main.ttf",
                14,
                5,
                {200, 200, 200}
            );
            updateWeaponDisplay();

            struct BtnDef { std::string key; std::function<void()> cb; };
            std::vector<BtnDef> buttons{
                {"menu.play", [this]() { _network.requestListRooms(); _changeState(GameState::Lobby); }},
                {"menu.singleplayer", [this]() { _network.tryStartSolo(); _changeState(GameState::RoomWaiting); }},
                {"menu.settings", [this]() { _changeState(GameState::Settings); }},
                {"menu.mods", [this]() { _changeState(GameState::ModMenu); }},
                {"menu.exit", [this]() { std::exit(0); }}
            };

            const float startX = 490.0f;
            const float startY = 300.0f;
            const float spacingY = 70.0f;
            const graphics::size btnSize{300.0f, 60.0f};

            for (size_t i = 0; i < buttons.size(); ++i) {
                float y = startY + static_cast<float>(i) * spacingY;
                _uiFactory.createButton(_uiRegistry,
                                        {startX, y},
                                        btnSize,
                                        _translationManager.get(buttons[i].key),
                                        buttons[i].cb);
            }
        }

        void MenuScene::onExit(void)
        {
            log::info("Exiting MenuScene");
            // La musique sera arrêtée par Application::stopAllUiSounds()
        }

        void MenuScene::handleEvent(const sf::Event& e)
        {
            if (const auto* kp = e.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    std::exit(0);
                }
            }
        }

        void MenuScene::update(float dt)
        {
            (void)dt;
        }

        void MenuScene::updateWeaponDisplay()
        {
            auto weapon = _settings.getSelectedWeapon();
            
            auto texts = _uiRegistry.get<ecs::components::ui::Text>();
            if (texts && texts.value().get().has(_weaponNameText)) {
                auto& text = texts.value().get()[_weaponNameText];
                // Prefer configured display name if available
                if (rtp::config::hasWeaponConfigs()) {
                    // WeaponConfig provides getWeaponDisplayName()
                    auto dn = rtp::config::getWeaponDisplayName(weapon);
                    if (!dn.empty()) text.content = dn;
                    else text.content = _settings.getWeaponName(weapon);
                } else {
                    text.content = _settings.getWeaponName(weapon);
                }
            }

            std::string stats;
            if (rtp::config::hasWeaponConfigs()) {
                auto def = rtp::config::getWeaponDef(weapon);
                stats += "Damage: " + std::to_string(def.damage) + "\n";
                if (def.maxAmmo < 0 || def.ammo < 0) {
                    stats += "Ammo: Infinite\n";
                } else {
                    stats += "Ammo: " + std::to_string(def.maxAmmo) + "\n";
                }
                if (def.fireRate == 0.0f && def.beamDuration > 0.0f) {
                    stats += "Fire Rate: Continuous\n";
                } else if (def.fireRate >= 5.0f) {
                    stats += "Fire Rate: High\n";
                } else if (def.fireRate >= 2.0f) {
                    stats += "Fire Rate: Medium\n";
                } else {
                    stats += "Fire Rate: Slow\n";
                }
                std::string special;
                if (def.beamDuration > 0.0f) {
                    special += "Beam: " + std::to_string(def.beamDuration) + "s active / " + std::to_string(def.beamCooldown) + "s cooldown";
                }
                if (def.canReflect) {
                    if (!special.empty()) special += " / ";
                    special += "Reflects enemy bullets";
                }
                if (def.homing) {
                    if (!special.empty()) special += " / ";
                    special += "Auto-homing shots";
                }
                if (def.isBoomerang) {
                    if (!special.empty()) special += " / ";
                    special += "Returns to player";
                }
                if (special.empty()) special = "Standard shots";
                stats += "Special: " + special + "\n";
                stats += "Difficulty: " + std::to_string(def.difficulty) + "/5";
            } else {
                log::warning("No weapon configurations found; using default stats display");
            }
            // else {
            //     switch (weapon) {
            //         case ecs::components::WeaponKind::Classic:
            //             stats =
            //                 "Damage: 10\n"
            //                 "Ammo: 100\n"
            //                 "Fire Rate: High\n"
            //                 "Special: Standard shots\n"
            //                 "Difficulty: 2/5";
            //             break;
            //         case ecs::components::WeaponKind::Beam:
            //             stats =
            //                 "Damage: 4 (per tick)\n"
            //                 "Ammo: 1\n"
            //                 "Fire Rate: Continuous\n"
            //                 "Special: Beam — 5s active / 5s cooldown\n"
            //                 "Difficulty: 3/5";
            //             break;
            //         case ecs::components::WeaponKind::Paddle:
            //             stats =
            //                 "Damage: 0 (reflects bullets)\n"
            //                 "Ammo: N/A\n"
            //                 "Fire Rate: N/A\n"
            //                 "Special: Reflects enemy bullets in front\n"
            //                 "Difficulty: 5/5";
            //             break;
            //         case ecs::components::WeaponKind::Tracker:
            //             stats =
            //                 "Damage: 6\n"
            //                 "Ammo: 50\n"
            //                 "Fire Rate: Medium\n"
            //                 "Special: Auto-homing shots\n"
            //                 "Difficulty: 1/5";
            //             break;
            //         case ecs::components::WeaponKind::Boomerang:
            //             stats =
            //                 "Damage: 18\n"
            //                 "Ammo: Infinite\n"
            //                 "Fire Rate: Slow\n"
            //                 "Special: Single projectile that returns to player\n"
            //                 "Difficulty: 4/5";
            //             break;
            //     }
            // }

            if (texts && texts.value().get().has(_weaponStatsText)) {
                auto& text = texts.value().get()[_weaponStatsText];
                text.content = stats;
            }
        }

    } // namespace scenes
} // namespace rtp::client