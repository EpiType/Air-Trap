/**
 * File   : MenuScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/MenuScene.hpp"
#include "RType/ECS/Components/Audio/AudioSource.hpp"
#include <vector>

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

    } // namespace scenes
} // namespace rtp::client