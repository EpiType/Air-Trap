/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
**
 * Application.cpp, main application loop
*/

#include "Core/Application.hpp"

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
        _worldSystemManager.add<client::NetworkSyncSystem>(_clientNetwork, _worldRegistry, _worldEntityBuilder);
        _worldSystemManager.add<client::InputSystem>(_worldRegistry, _uiRegistry, _settings, _clientNetwork, _window);
        _worldSystemManager.add<client::ParallaxSystem>(_worldRegistry);
        _worldSystemManager.add<client::AnimationSystem>(_worldRegistry);
        _worldSystemManager.add<client::RenderSystem>(_worldRegistry, _window);
        _worldSystemManager.add<client::ParallaxSystem>(_worldRegistry);
        log::info("OK: World systems initialized");
    }

    void Application::initUiSystems(void)
    {
        _uiSystemManager.add<client::UISystem>(_uiRegistry, _window, _settings);
        // _uiSystemManager.add<Client::Systems::SettingsMenuSystem>(_uiRegistry, _window, _settings);
        _uiSystemManager.add<Client::Systems::UIRenderSystem>(_uiRegistry, _window);
        log::info("OK: UI systems initialized");
    }

    void Application::initUiECS(void)
    {
        _uiRegistry.subscribe<ecs::components::ui::Button>();
        _uiRegistry.subscribe<ecs::components::ui::Text>();
        _uiRegistry.subscribe<ecs::components::ui::Slider>();
        _uiRegistry.subscribe<ecs::components::ui::Dropdown>();
        _uiRegistry.subscribe<ecs::components::ui::TextInput>();
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
        log::info("OK: World ECS initialized with components");
    }

    void Application::initScenes()
    {
        auto changeStateCb = [this](GameState s) { this->changeState(s); };
        try {
            auto& net = _worldSystemManager.getSystem<client::NetworkSyncSystem>();

        _scenes[GameState::Login] = std::make_unique<client::Scenes::LoginScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::Menu] = std::make_unique<client::Scenes::MenuScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::Lobby] = std::make_unique<client::Scenes::LobbyScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::CreateRoom] = std::make_unique<client::Scenes::CreateRoomScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::RoomWaiting] = std::make_unique<client::Scenes::RoomWaitingScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::Settings] = std::make_unique<client::Scenes::SettingsScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::KeyBindings] = std::make_unique<client::Scenes::KeyBindingScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::GamepadSettings] = std::make_unique<client::Scenes::GamepadSettingsScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::Paused] = std::make_unique<client::Scenes::PauseScene>(
            _uiRegistry, _settings, _translations, net, _uiFactory, changeStateCb
        );
        _scenes[GameState::Playing] = std::make_unique<client::Scenes::PlayingScene>(
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
        
        _uiRegistry.clear();
        
        if (newState != GameState::Playing && newState != GameState::Paused) {
            _worldRegistry.clear();
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
            // TODO: Appliquer au AudioManager quand il sera implémenté
        });

        _settings.onMusicVolumeChanged([this](float volume) {
            log::info("Music volume changed to: {:.2f}", volume);
            // TODO: Appliquer au AudioManager quand il sera implémenté
        });

        _settings.onSfxVolumeChanged([this](float volume) {
            log::info("SFX volume changed to: {:.2f}", volume);
            // TODO: Appliquer au AudioManager quand il sera implémenté
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

        _uiSystemManager.getSystem<Client::Systems::UIRenderSystem>().update(_lastDt);
        
        // Only show gamepad cursor in menus, not in-game (Playing state)
        if (_currentState != GameState::Playing) {
            _uiSystemManager.getSystem<UISystem>().renderGamepadCursor(_window);
        }

        _window.display();
    }
}
