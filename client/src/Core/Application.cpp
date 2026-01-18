/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
**
 * Application.cpp, main application loop
*/

#include "Core/Application.hpp"
#include "Game/SpriteCustomizer.hpp"
#include "RType/ECS/Components/ShieldVisual.hpp"

namespace rtp::client
{

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    Application::Application(const std::string& serverIp, uint16_t serverPort)
        : _window(sf::VideoMode(
                      {static_cast<unsigned int>(UIConstants::WINDOW_WIDTH),
                       static_cast<unsigned int>(UIConstants::WINDOW_HEIGHT)}),
                  "Air-Trap")
        , _uiSystemManager(_uiRegistry)
        , _worldSystemManager(_worldRegistry)
        , _uiEntityBuilder(_uiRegistry)
        , _worldEntityBuilder(_worldRegistry)
        , _clientNetwork(serverIp, serverPort)
    {
        _window.setFramerateLimit(60);

        _translations.loadLanguage(_settings.getLanguage());

        setupSettingsCallbacks();

        if (!_colorblindShader.loadFromFile("assets/shaders/colorblind.frag",
                                            sf::Shader::Type::Fragment)) {
            log::warning("Failed to load colorblind shader, running "
                              "without colorblind support");
            _shaderLoaded = false;
        } else {
            _shaderLoaded = true;
            log::info("Colorblind shader loaded successfully");
        }

        if (!_renderTexture.resize(
                {static_cast<unsigned int>(UIConstants::WINDOW_WIDTH),
                 static_cast<unsigned int>(UIConstants::WINDOW_HEIGHT)})) {
            log::error("Failed to create render texture");
        }

        // _audioManager.setMasterVolume(_settings.getMasterVolume());
        // _audioManager.setMusicVolume(_settings.getMusicVolume());
        // _audioManager.setSfxVolume(_settings.getSfxVolume());

        try {
            _clientNetwork.start();
        } catch (const std::exception& e) {
            throw std::runtime_error(
                std::string("Failed to start client network: ") + e.what());
        }
        initUiSystems();
        initWorldSystems();
        initUiECS();
        initWorldECS();
        initScenes();
        changeState(GameState::Login);
    }

    void Application::run(void)
    {
        sf::Clock clock;
        while (_window.isOpen()) {
            sf::Time deltaTime = clock.restart();
            processInput();
            update(deltaTime);
            render();
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void Application::initWorldSystems(void)
    {
        _worldSystemManager.add<NetworkSyncSystem>(_clientNetwork, _worldRegistry, _worldEntityBuilder);
        _worldSystemManager.add<InputSystem>(_worldRegistry, _uiRegistry, _settings, _clientNetwork, _window);
        _worldSystemManager.add<ParallaxSystem>(_worldRegistry);
        _worldSystemManager.add<AnimationSystem>(_worldRegistry);
        _worldSystemManager.add<ShieldSystem>(_worldRegistry);
        auto& worldAudioSystem = _worldSystemManager.add<AudioSystem>(_worldRegistry);
        worldAudioSystem.setMasterVolume(_settings.getMasterVolume());
        worldAudioSystem.setMusicVolume(_settings.getMusicVolume());
        worldAudioSystem.setSfxVolume(_settings.getSfxVolume());
        auto& worldRenderSystem = _worldSystemManager.add<RenderSystem>(_worldRegistry, _window);
        _worldSystemManager.add<ParallaxSystem>(_worldRegistry);
        
        // Update SpriteCustomizer with both render systems for cache clearing
        auto& uiRenderSystem = _uiSystemManager.getSystem<systems::UIRenderSystem>();
        SpriteCustomizer::setRenderSystems(&worldRenderSystem, &uiRenderSystem);
        
        log::info("OK: World systems initialized");
    }

    void Application::initUiSystems(void)
    {
        _uiSystemManager.add<UISystem>(_uiRegistry, _window, _settings);
        auto& uiAudioSystem = _uiSystemManager.add<AudioSystem>(_uiRegistry);
        uiAudioSystem.setMasterVolume(_settings.getMasterVolume());
        uiAudioSystem.setMusicVolume(_settings.getMusicVolume());
        uiAudioSystem.setSfxVolume(_settings.getSfxVolume());
        // _uiSystemManager.add<systems::SettingsMenuSystem>(_uiRegistry, _window, _settings);
        auto& uiRenderSystem = _uiSystemManager.add<systems::UIRenderSystem>(_uiRegistry, _window);
        
        // Register UI render system with SpriteCustomizer for cache clearing
        SpriteCustomizer::setRenderSystems(nullptr, &uiRenderSystem);
        
        log::info("OK: UI systems initialized");
    }

    void Application::initUiECS(void)
    {
        _uiRegistry.subscribe<ecs::components::ui::Button>();
        _uiRegistry.subscribe<ecs::components::ui::Text>();
        _uiRegistry.subscribe<ecs::components::ui::Slider>();
        _uiRegistry.subscribe<ecs::components::ui::Dropdown>();
        _uiRegistry.subscribe<ecs::components::ui::TextInput>();
        _uiRegistry.subscribe<ecs::components::ui::SpritePreview>();
        _uiRegistry.subscribe<ecs::components::audio::AudioSource>();
        _uiRegistry.subscribe<ecs::components::audio::SoundEvent>();
        log::info("OK: UI ECS initialized with components");
    }

    void Application::initWorldECS(void)
    {
        _worldRegistry.subscribe<ecs::components::Transform>();
        _worldRegistry.subscribe<ecs::components::Velocity>();
        _worldRegistry.subscribe<ecs::components::Controllable>();
        _worldRegistry.subscribe<ecs::components::Sprite>();
        _worldRegistry.subscribe<ecs::components::Animation>();
        _worldRegistry.subscribe<ecs::components::ParallaxLayer>();
        _worldRegistry.subscribe<ecs::components::NetworkId>();
        _worldRegistry.subscribe<ecs::components::EntityType>();
        _worldRegistry.subscribe<ecs::components::BoundingBox>();
        _worldRegistry.subscribe<ecs::components::ShieldVisual>();
        _worldRegistry.subscribe<ecs::components::audio::AudioSource>();
        _worldRegistry.subscribe<ecs::components::audio::SoundEvent>();
        log::info("OK: World ECS initialized with components");
    }

    void Application::initScenes()
    {
        auto changeStateCb = [this](GameState s) { this->changeState(s); };
        try {
            auto& net = _worldSystemManager.getSystem<NetworkSyncSystem>();

        _scenes[GameState::Login] = std::make_unique<scenes::LoginScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::Menu] = std::make_unique<scenes::MenuScene>(
            _uiRegistry, _worldRegistry, _settings, _translations, net, _uiFactory, _worldEntityBuilder, changeStateCb
        );
        _scenes[GameState::Lobby] = std::make_unique<scenes::LobbyScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::CreateRoom] = std::make_unique<scenes::CreateRoomScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::RoomWaiting] = std::make_unique<scenes::RoomWaitingScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::Settings] = std::make_unique<scenes::SettingsScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::KeyBindings] = std::make_unique<scenes::KeyBindingScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::GamepadSettings] = std::make_unique<scenes::GamepadSettingsScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::ModMenu] = std::make_unique<scenes::ModMenuScene>(
            _uiRegistry, _settings, _translations, _uiFactory, changeStateCb
        );
        _scenes[GameState::Paused] = std::make_unique<scenes::PauseScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::GameOver] = std::make_unique<scenes::GameOverScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::Playing] = std::make_unique<scenes::PlayingScene>(
            _worldRegistry, _uiRegistry, _settings, _translations, net, _uiFactory, _worldEntityBuilder, changeStateCb
        );
        } catch (const std::exception& e) {
            log::error("Failed to get NetworkSyncSystem: {}", e.what());
            throw;
        }

        log::info("OK: Scenes initialized");
    }

    void Application::changeState(GameState newState)
    {
        log::info("Changing state from {} to {}",
                    static_cast<int>(_currentState),
                    static_cast<int>(newState));

        if (newState == _currentState) return;

        if (_activeScene) {
            _activeScene->onExit();
        }

        if (_currentState == GameState::Playing && newState != GameState::Playing) {
            auto &worldAudio = _worldSystemManager.getSystem<AudioSystem>();
            worldAudio.stopAllSounds();
        }

            if (newState == GameState::Playing) {
                auto &uiAudio = _uiSystemManager.getSystem<AudioSystem>();
                uiAudio.stopAllSounds();
                _uiRegistry.clear();
            } else {
                std::vector<ecs::Entity> audioSourcesToKeep;
                std::vector<ecs::components::audio::AudioSource> audioSourceData;
            
                auto audioSources = _uiRegistry.get<ecs::components::audio::AudioSource>();
                if (audioSources) {
                    auto& sources = audioSources.value().get();
                    for (const auto& entity : sources.entities()) {
                        audioSourcesToKeep.push_back(entity);
                        audioSourceData.push_back(sources[entity]);
                    }
                }

                _uiRegistry.clear();

                for (size_t i = 0; i < audioSourcesToKeep.size(); ++i) {
                    _uiRegistry.add<ecs::components::audio::AudioSource>(audioSourcesToKeep[i], audioSourceData[i]);
                }
            }
        
        if (newState != GameState::Playing && newState != GameState::Paused) {
            _worldRegistry.clear();
        }

        // Clear texture caches when leaving ModMenu or entering Playing
        // This ensures custom sprite changes (including resets) are applied immediately
        if (_currentState == GameState::ModMenu || newState == GameState::Playing) {
            log::info("Clearing texture caches to apply sprite changes");
            _worldSystemManager.getSystem<RenderSystem>().clearTextureCache();
            _uiSystemManager.getSystem<systems::UIRenderSystem>().clearTextureCache();
        }

        auto it = _scenes.find(newState);
        if (it != _scenes.end()) {
            _activeScene = it->second.get();
            _activeScene->onEnter();
        } else {
            log::warning("No scene found for state {}, cannot change state",
                            static_cast<int>(newState));
            return;
        }

        _currentState = newState;
    }

    void Application::processInput()
    {
        while (auto eventOpt = _window.pollEvent()) {
            const sf::Event& event = *eventOpt;

            if (event.is<sf::Event::Closed>()) {
                _window.close();
                continue;
            }

            if (_activeScene)
                _activeScene->handleEvent(event);

            _uiSystemManager.getSystem<UISystem>().handleEvent(event);
        }
    }

    void Application::update(sf::Time delta)
    {
        _lastDt = delta.asSeconds();
        const float dt = _lastDt;

        if (_activeScene) {
            _activeScene->update(dt);
        }

        _worldSystemManager.update(dt);

        _uiSystemManager.update(dt);
    }


    void Application::setupSettingsCallbacks()
    {
        _settings.onMasterVolumeChanged([this](float volume) {
            log::info("Master volume changed to: {:.2f}", volume);
            _worldSystemManager.getSystem<AudioSystem>().setMasterVolume(volume);
            _uiSystemManager.getSystem<AudioSystem>().setMasterVolume(volume);
        });

        _settings.onMusicVolumeChanged([this](float volume) {
            log::info("Music volume changed to: {:.2f}", volume);
            _worldSystemManager.getSystem<AudioSystem>().setMusicVolume(volume);
            _uiSystemManager.getSystem<AudioSystem>().setMusicVolume(volume);
        });

        _settings.onSfxVolumeChanged([this](float volume) {
            log::info("SFX volume changed to: {:.2f}", volume);
            _worldSystemManager.getSystem<AudioSystem>().setSfxVolume(volume);
            _uiSystemManager.getSystem<AudioSystem>().setSfxVolume(volume);
        });

        _settings.onLanguageChanged([this](Language lang) {
            log::info("Language changed to: {}", static_cast<int>(lang));

            _translations.loadLanguage(lang);

            if (_activeScene) {
                _uiRegistry.clear();
                _activeScene->onEnter();
            }
        });
    }

    void Application::render()
    {
        _window.clear(sf::Color::Black);

        _worldSystemManager.getSystem<client::RenderSystem>().update(_lastDt);

        if (_shaderLoaded && _settings.getColorBlindMode() != ColorBlindMode::None) {
            sf::RectangleShape overlay({UIConstants::WINDOW_WIDTH, UIConstants::WINDOW_HEIGHT});
            overlay.setPosition({0.0f, 0.0f});

            switch (_settings.getColorBlindMode()) {
                case ColorBlindMode::Protanopia:   overlay.setFillColor(sf::Color(255, 150, 100, 30)); break;
                case ColorBlindMode::Deuteranopia: overlay.setFillColor(sf::Color(200, 255, 100, 30)); break;
                case ColorBlindMode::Tritanopia:   overlay.setFillColor(sf::Color(255, 100, 200, 30)); break;
                default: break;
            }
            _window.draw(overlay);
        }

        _uiSystemManager.getSystem<systems::UIRenderSystem>().update(_lastDt);
        
        // Only show gamepad cursor in menus, not in-game (Playing state)
        if (_currentState != GameState::Playing) {
            _uiSystemManager.getSystem<UISystem>().renderGamepadCursor(_window);
        }

        _window.display();
    }
}
