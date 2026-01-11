/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
**
 * Application.cpp, main application loop
*/

#include "Core/Application.hpp"

#include "Core/Settings.hpp"
#include "RType/Logger.hpp"
#include "Systems/MenuSystem.hpp"
#include "Systems/SettingsMenuSystem.hpp"
#include "Systems/UIRenderSystem.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Clock.hpp>
#include <memory>
// ECS
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/SystemManager.hpp"

// Components
#include "RType/ECS/Components/Animation.hpp"
#include "RType/ECS/Components/Controllable.hpp"
#include "RType/ECS/Components/NetworkId.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Dropdown.hpp"
#include "RType/ECS/Components/UI/Slider.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/Velocity.hpp"

// Systems
#include "RType/Math/Vec2.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Utils/GameState.hpp"
#include "Systems/NetworkSyncSystem.hpp"
#include "Systems/InputSystem.hpp"
#include "Systems/ParallaxSystem.hpp"
#include "Systems/RenderSystem.hpp"

namespace Client::Core
{
    Application::Application()
        : _window(sf::VideoMode(
                      {static_cast<unsigned int>(UIConstants::WINDOW_WIDTH),
                       static_cast<unsigned int>(UIConstants::WINDOW_HEIGHT)}),
                  "Air-Trap - R-Type Clone")
        , _systemManager(_registry)
        , _entityBuilder(_registry)
        , _clientNetwork("127.0.0.1", 5000)
    {
        _window.setFramerateLimit(60);

        _translations.loadLanguage(_settings.getLanguage());

        setupSettingsCallbacks();

        // Load colorblind shader
        if (!_colorblindShader.loadFromFile("assets/shaders/colorblind.frag",
                                            sf::Shader::Type::Fragment)) {
            rtp::log::warning("Failed to load colorblind shader, running "
                              "without colorblind support");
            _shaderLoaded = false;
        } else {
            _shaderLoaded = true;
            rtp::log::info("Colorblind shader loaded successfully");
        }

        // Create render texture for post-processing
        if (!_renderTexture.resize(
                {static_cast<unsigned int>(UIConstants::WINDOW_WIDTH),
                 static_cast<unsigned int>(UIConstants::WINDOW_HEIGHT)})) {
            rtp::log::error("Failed to create render texture");
        }

        // _audioManager.setMasterVolume(_settings.getMasterVolume());
        // _audioManager.setMusicVolume(_settings.getMusicVolume());
        // _audioManager.setSfxVolume(_settings.getSfxVolume());

        _clientNetwork.start();
        initECS();
        changeState(_currentState);
    }

    void Application::run()
    {
        sf::Clock clock;
        while (_window.isOpen()) {
            sf::Time deltaTime = clock.restart();
            processInput();
            update(deltaTime);
            render();
        }
    }

    void Application::initECS()
    {
        _registry.registerComponent<rtp::ecs::components::ui::Button>();
        _registry.registerComponent<rtp::ecs::components::ui::Text>();
        _registry.registerComponent<rtp::ecs::components::ui::Slider>();
        _registry.registerComponent<rtp::ecs::components::ui::Dropdown>();
        _registry.registerComponent<rtp::ecs::components::ui::TextInput>();

        _registry.registerComponent<rtp::ecs::components::Transform>();
        _registry.registerComponent<rtp::ecs::components::Velocity>();
        _registry.registerComponent<rtp::ecs::components::Controllable>();
        _registry.registerComponent<rtp::ecs::components::Sprite>();
        _registry.registerComponent<rtp::ecs::components::Animation>();
        _registry.registerComponent<rtp::ecs::components::ParallaxLayer>();

        _systemManager.addSystem<rtp::client::ParallaxSystem>(_registry);
        _registry.registerComponent<rtp::ecs::components::NetworkId>();

        _systemManager.addSystem<rtp::client::InputSystem>(
            _registry, _settings, _clientNetwork, _window);
        _systemManager.addSystem<rtp::client::AnimationSystem>(_registry);
        _systemManager.addSystem<rtp::client::RenderSystem>(_registry, _window);

        _systemManager.addSystem<Client::Systems::MenuSystem>(_registry,
                                                              _window);
        _systemManager.addSystem<Client::Systems::UIRenderSystem>(_registry,
                                                                  _window);
        _systemManager.addSystem<Client::Systems::SettingsMenuSystem>(
            _registry, _window, _settings);

        _systemManager.addSystem<rtp::client::ClientNetworkSystem>(
            _clientNetwork, _registry, _entityBuilder);
    }

    void Application::initMenu()
    {
        rtp::log::info("Initializing menu...");
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

        // NOTE: Menu entities are not deleted to avoid a crash in killEntity().
        // They remain in the ECS but are not rendered since only the
        // RenderSystem is active during gameplay (UIRenderSystem is only called
        // in Menu state). The menu buttons are still clickable but their
        // callbacks won't do anything harmful.
        // TODO: Fix the killEntity() implementation to properly handle entity
        // deletion, or implement a tag-based filtering system for systems.

        // Spawn player
        // auto playerRes = _registry.spawnEntity();
        // if (!playerRes) return;
        // rtp::ecs::Entity p = playerRes.value();

        // _registry.addComponent<rtp::ecs::components::Transform>(
        //     p, rtp::Vec2f{UIConstants::PLAYER_SPAWN_X,
        //     UIConstants::PLAYER_SPAWN_Y}, 0.0f, rtp::Vec2f{1.0f, 1.0f});
        // _registry.addComponent<rtp::ecs::components::Velocity>(p);
        // _registry.addComponent<rtp::ecs::components::Controllable>(p);

        // rtp::ecs::components::Sprite spriteData;
        // spriteData.texturePath = "assets/sprites/r-typesheet42.gif";
        // spriteData.rectLeft = 0;
        // spriteData.rectTop = 0;
        // spriteData.rectWidth = 33;
        // spriteData.rectHeight = 17;
        // spriteData.zIndex = 10;
        // spriteData.red = 255;

        // _registry.addComponent<rtp::ecs::components::Sprite>(p, spriteData);

        // rtp::ecs::components::Animation animData;
        // animData.frameLeft = 0;
        // animData.frameTop = 0;
        // animData.frameWidth = 33;
        // animData.frameHeight = 17;
        // animData.totalFrames = 5;
        // animData.frameDuration = 0.1f;
        // animData.currentFrame = 0;
        // animData.elapsedTime = 0.0f;

        // _registry.addComponent<rtp::ecs::components::Animation>(p, animData);
    }

    void Application::initPauseMenu()
    {
        rtp::log::info("Initializing pause menu...");

    }

    void Application::initLoginScene()
    {
        rtp::log::info("Initializing Login scene...");

    }

    void Application::initLobbyScene()
    {
        rtp::log::info("Initializing Lobby scene...");

    }

    void Application::initCreateRoomScene()
    {
        (void)_registry;
    }

    void Application::initRoomWaitingScene()
    {
        rtp::log::info("Initializing RoomWaiting scene...");
    }

    void Application::initKeyBindingMenu()
    {
        rtp::log::info("Initializing key bindings menu...");

    }

    void Application::initSettingsMenu()
    {
        rtp::log::info("Initializing settings menu...");

        // Title
        auto titleResult = _registry.spawnEntity();
        if (titleResult) {
            rtp::ecs::Entity title = titleResult.value();
            rtp::ecs::components::ui::Text titleText;
            titleText.content = _translations.get("settings.title");
            titleText.position = rtp::Vec2f{500.0f, 50.0f};
            titleText.fontPath = "assets/fonts/main.ttf";
            titleText.fontSize = 60;
            titleText.red = 255;
            titleText.green = 200;
            titleText.blue = 100;
            _registry.addComponent<rtp::ecs::components::ui::Text>(title,
                                                                   titleText);
        }

        // === AUDIO SECTION ===
        float yPos = 150.0f;

        // Master Volume Label
        auto masterLabelRes = _registry.spawnEntity();
        if (masterLabelRes) {
            rtp::ecs::Entity label = masterLabelRes.value();
            rtp::ecs::components::ui::Text text;
            text.content = _translations.get("settings.master_volume");
            text.position = rtp::Vec2f{200.0f, yPos};
            text.fontPath = "assets/fonts/main.ttf";
            text.fontSize = 24;
            _registry.addComponent<rtp::ecs::components::ui::Text>(label, text);
        }

        // Master Volume Slider
        auto masterSliderRes = _registry.spawnEntity();
        if (masterSliderRes) {
            rtp::ecs::Entity slider = masterSliderRes.value();
            rtp::ecs::components::ui::Slider sliderComp;
            sliderComp.position = rtp::Vec2f{450.0f, yPos + 5.0f};
            sliderComp.size = rtp::Vec2f{300.0f, 15.0f};
            sliderComp.minValue = 0.0f;
            sliderComp.maxValue = 1.0f;
            sliderComp.currentValue = _settings.getMasterVolume();
            sliderComp.onChange = [this](float value) {
                _settings.setMasterVolume(value);
            };
            _registry.addComponent<rtp::ecs::components::ui::Slider>(
                slider, sliderComp);
        }

        yPos += 50.0f;

        // Music Volume Label
        auto musicLabelRes = _registry.spawnEntity();
        if (musicLabelRes) {
            rtp::ecs::Entity label = musicLabelRes.value();
            rtp::ecs::components::ui::Text text;
            text.content = _translations.get("settings.music_volume");
            text.position = rtp::Vec2f{200.0f, yPos};
            text.fontPath = "assets/fonts/main.ttf";
            text.fontSize = 24;
            _registry.addComponent<rtp::ecs::components::ui::Text>(label, text);
        }

        // Music Volume Slider
        auto musicSliderRes = _registry.spawnEntity();
        if (musicSliderRes) {
            rtp::ecs::Entity slider = musicSliderRes.value();
            rtp::ecs::components::ui::Slider sliderComp;
            sliderComp.position = rtp::Vec2f{450.0f, yPos + 5.0f};
            sliderComp.size = rtp::Vec2f{300.0f, 15.0f};
            sliderComp.currentValue = _settings.getMusicVolume();
            sliderComp.onChange = [this](float value) {
                _settings.setMusicVolume(value);
            };
            _registry.addComponent<rtp::ecs::components::ui::Slider>(
                slider, sliderComp);
        }

        yPos += 50.0f;

        // SFX Volume Label
        auto sfxLabelRes = _registry.spawnEntity();
        if (sfxLabelRes) {
            rtp::ecs::Entity label = sfxLabelRes.value();
            rtp::ecs::components::ui::Text text;
            text.content = _translations.get("settings.sfx_volume");
            text.position = rtp::Vec2f{200.0f, yPos};
            text.fontPath = "assets/fonts/main.ttf";
            text.fontSize = 24;
            _registry.addComponent<rtp::ecs::components::ui::Text>(label, text);
        }

        // SFX Volume Slider
        auto sfxSliderRes = _registry.spawnEntity();
        if (sfxSliderRes) {
            rtp::ecs::Entity slider = sfxSliderRes.value();
            rtp::ecs::components::ui::Slider sliderComp;
            sliderComp.position = rtp::Vec2f{450.0f, yPos + 5.0f};
            sliderComp.size = rtp::Vec2f{300.0f, 15.0f};
            sliderComp.currentValue = _settings.getSfxVolume();
            sliderComp.onChange = [this](float value) {
                _settings.setSfxVolume(value);
            };
            _registry.addComponent<rtp::ecs::components::ui::Slider>(
                slider, sliderComp);
        }

        yPos += 80.0f;

        // === LANGUAGE SECTION ===
        auto langLabelRes = _registry.spawnEntity();
        if (langLabelRes) {
            rtp::ecs::Entity label = langLabelRes.value();
            rtp::ecs::components::ui::Text text;
            text.content = _translations.get("settings.language");
            text.position = rtp::Vec2f{200.0f, yPos};
            text.fontPath = "assets/fonts/main.ttf";
            text.fontSize = 24;
            _registry.addComponent<rtp::ecs::components::ui::Text>(label, text);
        }

        // Language Dropdown
        auto langDropdownRes = _registry.spawnEntity();
        if (langDropdownRes) {
            rtp::ecs::Entity dropdown = langDropdownRes.value();
            rtp::ecs::components::ui::Dropdown dropdownComp;
            dropdownComp.position = rtp::Vec2f{450.0f, yPos - 5.0f};
            dropdownComp.size = rtp::Vec2f{300.0f, 35.0f};
            dropdownComp.options = {"English", "Français", "Español", "Deutsch",
                                    "Italiano"};
            dropdownComp.selectedIndex =
                static_cast<int>(_settings.getLanguage());
            dropdownComp.onSelect = [this](int index) {
                _settings.setLanguage(static_cast<Core::Language>(index));
                _translations.loadLanguage(_settings.getLanguage());
                changeState(GameState::Settings);
            };
            _registry.addComponent<rtp::ecs::components::ui::Dropdown>(
                dropdown, dropdownComp);
        }

        yPos += 80.0f;

        // === ACCESSIBILITY SECTION ===
        auto colorblindLabelRes = _registry.spawnEntity();
        if (colorblindLabelRes) {
            rtp::ecs::Entity label = colorblindLabelRes.value();
            rtp::ecs::components::ui::Text text;
            text.content = _translations.get("settings.colorblind_mode");
            text.position = rtp::Vec2f{200.0f, yPos};
            text.fontPath = "assets/fonts/main.ttf";
            text.fontSize = 24;
            _registry.addComponent<rtp::ecs::components::ui::Text>(label, text);
        }

        // Colorblind Mode Dropdown
        auto colorblindDropdownRes = _registry.spawnEntity();
        if (colorblindDropdownRes) {
            rtp::ecs::Entity dropdown = colorblindDropdownRes.value();
            rtp::ecs::components::ui::Dropdown dropdownComp;
            dropdownComp.position = rtp::Vec2f{450.0f, yPos - 5.0f};
            dropdownComp.size = rtp::Vec2f{300.0f, 35.0f};
            dropdownComp.options = {
                _translations.get("colorblind.none"),
                _translations.get("colorblind.protanopia"),
                _translations.get("colorblind.deuteranopia"),
                _translations.get("colorblind.tritanopia")};
            dropdownComp.selectedIndex =
                static_cast<int>(_settings.getColorBlindMode());
            dropdownComp.onSelect = [this](int index) {
                _settings.setColorBlindMode(
                    static_cast<Core::ColorBlindMode>(index));
                rtp::log::info("Colorblind mode changed to: {}", index);
            };
            _registry.addComponent<rtp::ecs::components::ui::Dropdown>(
                dropdown, dropdownComp);
        }

        yPos += 80.0f;

        auto keyBindingsBtnRes = _registry.spawnEntity();
        if (keyBindingsBtnRes) {
            rtp::ecs::Entity btn = keyBindingsBtnRes.value();

            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("settings.key_bindings");
            button.position = rtp::Vec2f{490.0f, yPos};
            button.size = rtp::Vec2f{300.0f, 60.0f};
            button.onClick = [this]() {
                rtp::log::info("Key Bindings button clicked!");
                changeState(GameState::KeyBindings);
            };

            _registry.addComponent<rtp::ecs::components::ui::Button>(btn,
                                                                     button);
        }

        // Back Button (déplacer plus bas)
        auto backBtnRes = _registry.spawnEntity();
        if (backBtnRes) {
            rtp::ecs::Entity backBtn = backBtnRes.value();

            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("settings.back");
            button.position = rtp::Vec2f{490.0f, 600.0f};
            button.size = rtp::Vec2f{300.0f, 60.0f};
            button.onClick = [this]() {
                rtp::log::info("Back button clicked!");
                _settings.save();
                changeState(GameState::Menu);
            };

            _registry.addComponent<rtp::ecs::components::ui::Button>(backBtn,
                                                                     button);
        }
    }

    void Application::changeState(GameState newState)
{
    rtp::log::info("Changing state from {} to {}",
                   static_cast<int>(_currentState),
                   static_cast<int>(newState));

    GameState previousState = _currentState;

    const bool keepWorld =
        (previousState == GameState::RoomWaiting) ||
        (previousState == GameState::Paused);

    _currentState = newState;

    if (!keepWorld) {
        _registry.clear();
    }

    switch (newState) {
        case GameState::Menu:
            initMenu();
            break;
        case GameState::Login:
            initLoginScene();
            break;
        case GameState::Lobby:
            initLobbyScene();
            break;
        case GameState::CreateRoom:
            initCreateRoomScene();
            break;
        case GameState::RoomWaiting:
            initRoomWaitingScene();
            break;
        case GameState::Settings:
            initSettingsMenu();
            break;
        case GameState::KeyBindings:
            initKeyBindingMenu();
            break;
        case GameState::Paused:
            initPauseMenu();
            break;
        case GameState::Playing:
            if (previousState != GameState::Paused) {
                initGame();
            }
            break;
        default:
            break;
    }
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

            switch (_currentState) {
                case GameState::Playing:
                    processGameInput(event.value());
                    break;

                case GameState::KeyBindings:
                    processKeyBindingInput(event.value());
                    break;

                default:
                    processMenuInput(event.value());
                    break;
            }
        }
    }

    void Application::processMenuInput(const sf::Event &event)
    {
        auto &menuSys = _systemManager.getSystem<Client::Systems::MenuSystem>();
        menuSys.handleEvent(event);
    }

    void Application::processGameInput(const sf::Event &event)
    {
        // Les inputs de debug (J/K/L/M) sont gérés dans processInput()
        // Les touches de mouvement/tir sont gérées par InputSystem
        (void)event; // Éviter le warning unused
    }

    void Application::processSettingsInput(const sf::Event &event)
    {
        // Le MenuSystem gère déjà les sliders/dropdowns
        (void)event;
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