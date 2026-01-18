/**
 * File   : MenuScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/MenuScene.hpp"
#include "Game/EntityBuilder.hpp"
#include "RType/ECS/Components/Audio/AudioSource.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include <vector>
#include <cmath>
#include "RType/Config/WeaponConfig.hpp"

namespace rtp::client {
    namespace scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        MenuScene::MenuScene(ecs::Registry& UiRegistry,
                             ecs::Registry& worldRegistry,
                             Settings& settings,
                             TranslationManager& translationManager,
                             NetworkSyncSystem& network,
                             graphics::UiFactory& uiFactory,
                             EntityBuilder& worldEntityBuilder,
                             std::function<void(GameState)> changeState)
            : _uiRegistry(UiRegistry),
              _worldRegistry(worldRegistry),
              _settings(settings),
              _translationManager(translationManager),
              _network(network),
              _uiFactory(uiFactory),
              _worldEntityBuilder(worldEntityBuilder),
              _changeState(changeState),
              _menuMusicEntity()
        {
        }

        void MenuScene::onEnter(void)
        {
            log::info("Entering MenuScene");
            _menuTime = 0.0f;
            _menuWorldEntities.clear();
            _menuEnemies.clear();

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

            auto spawnParallaxLayer = [this](const EntityTemplate& base) {
                EntityTemplate first = base;
                first.position = {0.0f, 0.0f};
                if (auto e = _worldEntityBuilder.spawn(first)) {
                    _menuWorldEntities.push_back(e.value());
                }

                EntityTemplate second = base;
                float width = base.parallax.textureWidth * base.scale.x;
                second.position = {width, 0.0f};
                if (auto e = _worldEntityBuilder.spawn(second)) {
                    _menuWorldEntities.push_back(e.value());
                }
            };

            spawnParallaxLayer(EntityTemplate::createParrallaxLvl1_1());
            spawnParallaxLayer(EntityTemplate::createParrallaxLvl1_2());

            struct EnemySpec {
                EntityTemplate (*create)(const Vec2f&);
                float startX;
                float baseY;
                float speed;
                float amplitude;
                float frequency;
                float phase;
                float scale;
            };

            const EnemySpec enemySpecs[] = {
                {EntityTemplate::enemy_1, 1350.0f, 180.0f, 32.0f, 10.0f, 1.2f, 0.0f, 1.2f},
                {EntityTemplate::enemy_2, 1550.0f, 260.0f, 46.0f, 14.0f, 1.4f, 1.2f, 1.1f},
                {EntityTemplate::enemy_1, 1750.0f, 380.0f, 38.0f, 12.0f, 1.1f, 2.1f, 1.0f},
                {EntityTemplate::enemy_2, 1950.0f, 520.0f, 54.0f, 16.0f, 1.0f, 3.0f, 1.25f}
            };

            for (const auto& spec : enemySpecs) {
                EntityTemplate t = spec.create({spec.startX, spec.baseY});
                t.scale = {spec.scale, spec.scale};
                if (auto e = _worldEntityBuilder.spawn(t)) {
                    ecs::Entity entity = e.value();
                    _menuWorldEntities.push_back(entity);
                    _menuEnemies.push_back(MenuEnemy{
                        entity,
                        spec.speed,
                        spec.baseY,
                        spec.amplitude,
                        spec.frequency,
                        spec.phase,
                        spec.startX
                    });
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

            // Weapon selection list (explicit available kinds)
            const std::vector<ecs::components::WeaponKind> availableKinds = {
                ecs::components::WeaponKind::Classic,
                ecs::components::WeaponKind::Beam,
                ecs::components::WeaponKind::Tracker,
                ecs::components::WeaponKind::Boomerang
            };

            // Left arrow button
            _uiFactory.createButton(
                _uiRegistry,
                {80.0f, 280.0f},
                {50.0f, 50.0f},
                "<",
                [this, availableKinds]() {
                    auto currentKind = _settings.getSelectedWeapon();
                    size_t idx = 0;
                    for (size_t i = 0; i < availableKinds.size(); ++i) {
                        if (availableKinds[i] == currentKind) { idx = i; break; }
                    }
                    if (idx == 0) idx = availableKinds.size() - 1;
                    else --idx;
                    _settings.setSelectedWeapon(availableKinds[idx]);
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
                [this, availableKinds]() {
                    auto currentKind = _settings.getSelectedWeapon();
                    size_t idx = 0;
                    for (size_t i = 0; i < availableKinds.size(); ++i) {
                        if (availableKinds[i] == currentKind) { idx = i; break; }
                    }
                    idx = (idx + 1) % availableKinds.size();
                    _settings.setSelectedWeapon(availableKinds[idx]);
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
            for (ecs::Entity entity : _menuWorldEntities) {
                _worldEntityBuilder.kill(entity);
            }
            _menuWorldEntities.clear();
            _menuEnemies.clear();
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
            _menuTime += dt;

            auto transformsOpt = _worldRegistry.get<ecs::components::Transform>();
            if (!transformsOpt) {
                return;
            }

            auto& transforms = transformsOpt.value().get();
            for (auto& enemy : _menuEnemies) {
                if (!transforms.has(enemy.entity)) {
                    continue;
                }
                auto& transform = transforms[enemy.entity];
                transform.position.x -= enemy.speed * dt;
                if (transform.position.x < -120.0f) {
                    transform.position.x = enemy.resetX;
                }
                transform.position.y = enemy.baseY +
                    std::sin(_menuTime * enemy.frequency + enemy.phase) * enemy.amplitude;
            }
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
                // Reflect feature removed
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

            if (texts && texts.value().get().has(_weaponStatsText)) {
                auto& text = texts.value().get()[_weaponStatsText];
                text.content = stats;
            }
        }

    } // namespace scenes
} // namespace rtp::client
