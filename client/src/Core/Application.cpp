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
    Application::Application()
        : _window(sf::VideoMode(
                      {static_cast<unsigned int>(UIConstants::WINDOW_WIDTH),
                       static_cast<unsigned int>(UIConstants::WINDOW_HEIGHT)}),
                  "Air-Trap - R-Type Clone")
        , _UiSystemManager(_uiRegistry)
        , _WorldSystemManager(_worldRegistry)
        , _uiEntityBuilder(_uiRegistry)
        , _worldEntityBuilder(_worldRegistry)
        , _clientNetwork("127.0.0.1", 5000)
    {
        _window.setFramerateLimit(60);

        _translations.loadLanguage(_settings.getLanguage());

        setupSettingsCallbacks();

        if (!_colorblindShader.loadFromFile("assets/shaders/colorblind.frag",
                                            sf::Shader::Type::Fragment)) {
            rtp::log::warning("Failed to load colorblind shader, running "
                              "without colorblind support");
            _shaderLoaded = false;
        } else {
            _shaderLoaded = true;
            rtp::log::info("Colorblind shader loaded successfully");
        }

        if (!_renderTexture.resize(
                {static_cast<unsigned int>(UIConstants::WINDOW_WIDTH),
                 static_cast<unsigned int>(UIConstants::WINDOW_HEIGHT)})) {
            rtp::log::error("Failed to create render texture");
        }

        // _audioManager.setMasterVolume(_settings.getMasterVolume());
        // _audioManager.setMusicVolume(_settings.getMusicVolume());
        // _audioManager.setSfxVolume(_settings.getSfxVolume());

        _clientNetwork.start();
        initSystems();
        initUiECS();
        initWorldECS();
        changeState(GameState::Login);
    }

    void Application::initSystems(void)
    {
        _systemManager.addSystem<rtp::client::ParallaxSystem>(_registry);
        _systemManager.addSystem<rtp::client::InputSystem>(_registry, _settings, _clientNetwork, _window);
        _systemManager.addSystem<rtp::client::AnimationSystem>(_registry);
        _systemManager.addSystem<rtp::client::RenderSystem>(_registry, _window);
        _systemManager.addSystem<rtp::client::UISystem>(_registry, _window);
        _systemManager.addSystem<Client::Systems::UIRenderSystem>(_registry, _window);
        _systemManager.addSystem<Client::Systems::SettingsMenuSystem>(_registry, _window, _settings);
        _systemManager.addSystem<rtp::client::ClientNetworkSystem>( _clientNetwork, _registry, _entityBuilder);
        rtp::log::info("OK : Systems initialized");
    }

    void Application::initUiECS(void)
    {
        _uiRegistry.registerComponent<rtp::ecs::components::ui::Button>();
        _uiRegistry.registerComponent<rtp::ecs::components::ui::Text>();
        _uiRegistry.registerComponent<rtp::ecs::components::ui::Slider>();
        _uiRegistry.registerComponent<rtp::ecs::components::ui::Dropdown>();
        _uiRegistry.registerComponent<rtp::ecs::components::ui::TextInput>();
        rtp::log::info("OK : UI ECS initialized with components");
    }

    void Application::initWorldECS(void)
    {
        _worldRegistry.registerComponent<rtp::ecs::components::Transform>();
        _worldRegistry.registerComponent<rtp::ecs::components::Velocity>();
        _worldRegistry.registerComponent<rtp::ecs::components::Controllable>();
        _worldRegistry.registerComponent<rtp::ecs::components::Sprite>();
        _worldRegistry.registerComponent<rtp::ecs::components::Animation>();
        _worldRegistry.registerComponent<rtp::ecs::components::ParallaxLayer>();
        _worldRegistry.registerComponent<rtp::ecs::components::NetworkId>();
        rtp::log::info("OK : World ECS initialized with components");
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

    void Application::createParallaxBackground()
    {
        const float textureWidth = 1280.0f;

        Game::EntityTemplate t1 = Game::EntityTemplate::createParrallaxLayer1();
        float scale1X = t1.scale.x;
        auto result1A = _entityBuilder.spawn(t1);
        if (result1A.has_value()) {
            _parallaxLayers.push_back(result1A.value());
        } else {
            rtp::log::error("Failed to spawn Layer 1A: {}",
                            result1A.error().message());
        }
        t1.position.x = textureWidth * scale1X;
        auto result1B = _entityBuilder.spawn(t1);
        if (result1B.has_value()) {
            _parallaxLayers.push_back(result1B.value());
        } else {
            rtp::log::error("Failed to spawn Layer 1B: {}",
                            result1B.error().message());
        }
        Game::EntityTemplate t2 = Game::EntityTemplate::createParrallaxLayer2();
        float scale2X = t2.scale.x;
        auto result2A = _entityBuilder.spawn(t2);
        if (result2A.has_value()) {
            _parallaxLayers.push_back(result2A.value());
        } else {
            rtp::log::error("Failed to spawn Layer 2A: {}",
                            result2A.error().message());
        }
        t2.position.x = textureWidth * scale2X;
        auto result2B = _entityBuilder.spawn(t2);
        if (result2B.has_value()) {
            _parallaxLayers.push_back(result2B.value());
        } else {
            rtp::log::error("Failed to spawn Layer 2B: {}",
                            result2B.error().message());
        }

        rtp::log::info("Parallax background initialized with {} layers.",
                       _parallaxLayers.size());
    }

    void Application::initGame()
    {
        rtp::log::info("Starting game...");
        createParallaxBackground();

        _hudInit = false;

        auto makeHudText = [&](const rtp::Vec2f& pos, unsigned size) -> rtp::ecs::Entity {
            auto eRes = _registry.spawnEntity();
            if (!eRes) return {};

            auto e = eRes.value();
            rtp::ecs::components::ui::Text t;
            t.content = "";
            t.position = pos;
            t.fontPath = "assets/fonts/main.ttf";
            t.fontSize = size;
            t.red = 255; t.green = 255; t.blue = 255;
            t.alpha = 255;
            t.zIndex = 999; // au-dessus
            _registry.addComponent<rtp::ecs::components::ui::Text>(e, t);
            return e;
        };

        // Coin haut-droit
        _hudPing     = makeHudText({980.0f, 15.0f}, 18);
        _hudFps      = makeHudText({980.0f, 40.0f}, 18);
        _hudScore    = makeHudText({980.0f, 65.0f}, 20);
        _hudEntities = makeHudText({980.0f, 90.0f}, 18);

        _hudInit = true;
    }

    void Application::changeState(GameState newState)
{
    rtp::log::info("Changing state from {} to {}",
                   static_cast<int>(_currentState),
                   static_cast<int>(newState));

    if (newState == _currentState) return;

    if (_newState != GameState::Playing) _worldRegistry.clear();

    auto it = _scenes.find(newState);
    if (it != _scenes.end()) {
        _activeScene = it->second.get();
        _activeScene->onEnter();
    } else {
        rtp::log::warning("No scene found for state {}, cannot change state",
                          static_cast<int>(newState));
        return;
    }

    _currentState = newState;

    _activeScene = nullptr;
}

    void Application::processInput()
    {
        while (auto event = _window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                _window.close();

            if (const auto *kp = event->getIf<sf::Event::KeyPressed>()) {
                if (kp->code ==
                    _settings.getKey(KeyAction::Pause) &&
                    _currentState != GameState::KeyBindings) {
                    handleGlobalEscape();
                    return;
                }

#ifdef DEBUG
                // Debug keys (spawn/kill) - Only in DEBUG build
                if (kp->code == sf::Keyboard::Key::J) {
                    rtp::Vec2f mousePos{
                        static_cast<float>(sf::Mouse::getPosition(_window).x),
                        static_cast<float>(sf::Mouse::getPosition(_window).y)};
                    spawnEnemy(mousePos);
                    rtp::log::debug("[DEBUG] Enemy spawned at ({}, {})",
                                    mousePos.x, mousePos.y);
                }
                if (kp->code == sf::Keyboard::Key::K) {
                    killEnemy(0);
                    rtp::log::debug("[DEBUG] Killed enemy at index 0");
                }
                if (kp->code == sf::Keyboard::Key::I) {
                    rtp::Vec2f mousePos{
                        static_cast<float>(sf::Mouse::getPosition(_window).x),
                        static_cast<float>(sf::Mouse::getPosition(_window).y)};
                    spawnEnemy2(mousePos);
                    rtp::log::debug("[DEBUG] Enemy2 spawned at ({}, {})",
                                    mousePos.x, mousePos.y);
                }
                if (kp->code == sf::Keyboard::Key::L) {
                    rtp::Vec2f mousePos{
                        static_cast<float>(sf::Mouse::getPosition(_window).x),
                        static_cast<float>(sf::Mouse::getPosition(_window).y)};
                    spawnProjectile(mousePos);
                    rtp::log::debug("[DEBUG] Projectile spawned at ({}, {})",
                                    mousePos.x, mousePos.y);
                }
                if (kp->code == sf::Keyboard::Key::M) {
                    killProjectile(0);
                    rtp::log::debug("[DEBUG] Killed projectile at index 0");
                }
#endif
            }
            auto &UISystem = _systemManager.getSystem<rtp::client::UISystem>();
            UISystem.handleEvent(event);
        }
    }

    void Application::processKeyBindingInput(const sf::Event &event)
    {
        if (!_isWaitingForKey)
            return;

        if (const auto *kp = event.getIf<sf::Event::KeyPressed>()) {
            // Escape pour annuler
            if (kp->code == sf::Keyboard::Key::Escape) {
                _isWaitingForKey = false;
                rtp::log::info("Key binding cancelled");
                return;
            }

            // Assigner la nouvelle touche
            _settings.setKey(_keyActionToRebind, kp->code);
            _isWaitingForKey = false;

            rtp::log::info("Key {} bound to action {}",
                           _settings.getKeyName(kp->code),
                           static_cast<int>(_keyActionToRebind));

            changeState(GameState::KeyBindings);
        }
    }

    void Application::handleGlobalEscape()
    {
        switch (_currentState) {
            case GameState::Menu:
                _window.close(); // Quitter le jeu
                break;

            case GameState::Playing:
                changeState(GameState::Paused); // Pause (ou Menu)
                break;

            case GameState::Settings:
            case GameState::KeyBindings:
                _settings.save();
                changeState(GameState::Menu); // Retour au menu
                break;

            case GameState::Paused:
                changeState(GameState::Playing); // Reprendre le jeu
                break;

            default:
                break;
        }
    }

    void Application::update(sf::Time delta)
{
    _lastDt = delta.asSeconds();

    _systemManager.getSystem<rtp::client::ClientNetworkSystem>().update(_lastDt);

    if (_currentState == GameState::Playing) {
        _systemManager.update(_lastDt);

        if (_hudInit) {
            auto textsOpt = _registry.getComponents<rtp::ecs::components::ui::Text>();
            if (textsOpt) {
                auto &texts = textsOpt.value().get();

                const float dtMs = _lastDt * 1000.0f;
                const float fps  = (_lastDt > 0.00001f) ? (1.0f / _lastDt) : 0.0f;

                if (texts.has(_hudPing))
                    texts[_hudPing].content = "dt: " + std::to_string((int)dtMs) + " ms";
                if (texts.has(_hudFps))
                    texts[_hudFps].content = "fps: " + std::to_string((int)fps);
                if (texts.has(_hudScore))
                    texts[_hudScore].content = "score: " + std::to_string(_score);
                if (texts.has(_hudEntities))
                    texts[_hudEntities].content = "entities: (debug)";
            }
        }
    }

    // Menus / UI logique (clicks etc.)
    _systemManager.getSystem<Client::Systems::MenuSystem>().update(_lastDt);

    if (_currentState == GameState::Login) {
        auto &netSys = _systemManager.getSystem<rtp::client::ClientNetworkSystem>();
        if (netSys.isLoggedIn())
            changeState(GameState::Menu);
    }

    if (_currentState == GameState::RoomWaiting) {
        auto &netSys = _systemManager.getSystem<rtp::client::ClientNetworkSystem>();
        if (netSys.isInGame())
            changeState(GameState::Playing);
    }

    if (_currentState == GameState::Settings || _currentState == GameState::KeyBindings) {
        _systemManager.getSystem<Client::Systems::SettingsMenuSystem>().update(_lastDt);
    }
}


    void Application::setupSettingsCallbacks()
    {
        _settings.onMasterVolumeChanged([this](float volume) {
            rtp::log::info("Master volume changed to: {:.2f}", volume);
            // TODO: Appliquer au AudioManager quand il sera implémenté
        });

        _settings.onMusicVolumeChanged([this](float volume) {
            rtp::log::info("Music volume changed to: {:.2f}", volume);
            // TODO: Appliquer au AudioManager quand il sera implémenté
        });

        _settings.onSfxVolumeChanged([this](float volume) {
            rtp::log::info("SFX volume changed to: {:.2f}", volume);
            // TODO: Appliquer au AudioManager quand il sera implémenté
        });

        _settings.onLanguageChanged([this](Language lang) {
            rtp::log::info("Language changed to: {}", static_cast<int>(lang));

            // Recréer le menu pour appliquer la nouvelle langue
            if (_currentState == GameState::Settings) {
                changeState(GameState::Settings);
            } else if (_currentState == GameState::Menu) {
                changeState(GameState::Menu);
            }
        });
    }

    void Application::spawnProjectile(const rtp::Vec2f &position)
    {
        std::size_t newIndex = _projectiles.size();

        Game::EntityTemplate createBulletEnemy =
            Game::EntityTemplate::createBulletEnemy(position);

        auto result = _entityBuilder.spawn(createBulletEnemy);

        if (result.has_value()) {
            rtp::ecs::Entity newEnemy = result.value();
            _projectiles.push_back(newEnemy);
            rtp::log::debug("Projectile spawned (ID: {}) at index {}. Total: "
                            "{} projectiles",
                            static_cast<std::uint32_t>(newEnemy), newIndex,
                            _projectiles.size());
        } else {
            rtp::log::error("Failed to spawn projectile: {}",
                            result.error().message());
        }
    }

    void Application::spawnEnemy(const rtp::Vec2f &position)
    {
        std::size_t newIndex = _spawnedEnemy.size();

        Game::EntityTemplate scoutTemplate =
            Game::EntityTemplate::rt1_1(position);

        auto result = _entityBuilder.spawn(scoutTemplate);

        if (result.has_value()) {
            rtp::ecs::Entity newEnemy = result.value();
            _spawnedEnemy.push_back(newEnemy);
            rtp::log::debug(
                "Enemy spawned (ID: {}) at index {}. Total: {} enemies",
                static_cast<std::uint32_t>(newEnemy), newIndex,
                _spawnedEnemy.size());
        } else {
            rtp::log::error("Failed to spawn enemy: {}",
                            result.error().message());
        }
    }

    void Application::spawnEnemy2(const rtp::Vec2f &position)
    {
        std::size_t newIndex = _spawnedEnemy.size();

        Game::EntityTemplate scoutTemplate =
            Game::EntityTemplate::createBasicScout2(position);

        auto result = _entityBuilder.spawn(scoutTemplate);

        if (result.has_value()) {
            rtp::ecs::Entity newEnemy = result.value();
            _spawnedEnemy.push_back(newEnemy);
            rtp::log::debug(
                "Enemy spawned (ID: {}) at index {}. Total: {} enemies",
                static_cast<std::uint32_t>(newEnemy), newIndex,
                _spawnedEnemy.size());
        } else {
            rtp::log::error("Failed to spawn enemy: {}",
                            result.error().message());
        }
    }

    void Application::killEnemy(std::size_t index)
    {
        if (index >= _spawnedEnemy.size()) {
            rtp::log::warning(
                "Cannot kill enemy: index {} out of bounds (0-{})", index,
                _spawnedEnemy.empty() ? 0 : _spawnedEnemy.size() - 1);
            return;
        }

        rtp::ecs::Entity entityToKill = _spawnedEnemy.at(index);

        _entityBuilder.kill(entityToKill);
        _spawnedEnemy.erase(
            _spawnedEnemy.begin() +
            static_cast<std::vector<rtp::ecs::Entity>::difference_type>(index));

        rtp::log::debug(
            "Enemy killed (ID: {}) at index {}. Remaining: {} enemies",
            static_cast<std::uint32_t>(entityToKill), index,
            _spawnedEnemy.size());
    }

    void Application::killProjectile(std::size_t index)
    {
        if (index >= _projectiles.size()) {
            rtp::log::warning(
                "Cannot kill projectile: index {} out of bounds (0-{})", index,
                _projectiles.empty() ? 0 : _projectiles.size() - 1);
            return;
        }

        rtp::ecs::Entity entityToKill = _projectiles.at(index);

        _entityBuilder.kill(entityToKill);
        _projectiles.erase(
            _projectiles.begin() +
            static_cast<std::vector<rtp::ecs::Entity>::difference_type>(index));

        rtp::log::debug(
            "Projectile killed (ID: {}) at index {}. Remaining: {} projectiles",
            static_cast<std::uint32_t>(entityToKill), index,
            _projectiles.size());
    }

    void Application::render()
{
    _window.clear(sf::Color::Black);

    if (_currentState == GameState::Playing) {
        _systemManager.getSystem<rtp::client::RenderSystem>().update(_lastDt);

        if (_shaderLoaded && _settings.getColorBlindMode() != ColorBlindMode::None) {
            sf::RectangleShape overlay(sf::Vector2f(
                UIConstants::WINDOW_WIDTH, UIConstants::WINDOW_HEIGHT));
            overlay.setPosition({0.0f, 0.0f});

            switch (_settings.getColorBlindMode()) {
                case ColorBlindMode::Protanopia:   overlay.setFillColor(sf::Color(255, 150, 100, 30)); break;
                case ColorBlindMode::Deuteranopia: overlay.setFillColor(sf::Color(200, 255, 100, 30)); break;
                case ColorBlindMode::Tritanopia:   overlay.setFillColor(sf::Color(255, 100, 200, 30)); break;
                default: break;
            }
            _window.draw(overlay);
        }

        _systemManager.getSystem<Client::Systems::UIRenderSystem>().update(_lastDt);

    } else {
        _systemManager.getSystem<Client::Systems::UIRenderSystem>().update(_lastDt);
    }

    _window.display();
}
}