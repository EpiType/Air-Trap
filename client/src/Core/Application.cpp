/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
** Application.cpp, main application loop
*/


#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Clock.hpp>

#include "Core/Application.hpp"
#include "Core/Settings.hpp"
#include "Systems/MenuSystem.hpp"
#include "Systems/UIRenderSystem.hpp"
#include "Systems/SettingsMenuSystem.hpp"
#include "RType/Logger.hpp"
// ECS
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/SystemManager.hpp"

// Components
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/Controllable.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Animation.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/UI/Slider.hpp"
#include "RType/ECS/Components/UI/Dropdown.hpp"
#include "RType/ECS/Components/NetworkId.hpp"

// Systems
#include "Systems/InputSystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/RenderSystem.hpp"
#include "Systems/ClientNetworkSystem.hpp"

#include "RType/Math/Vec2.hpp"

namespace Client::Core
{
    Application::Application()
        : _window(sf::VideoMode({static_cast<unsigned int>(UIConstants::WINDOW_WIDTH), 
                                 static_cast<unsigned int>(UIConstants::WINDOW_HEIGHT)}), 
                  "Air-Trap - R-Type Clone")
        , _systemManager(_registry), _entityBuilder(_registry), _clientNetwork("127.0.0.1", 12345)
    {
        _window.setFramerateLimit(60);

        _translations.loadLanguage(_settings.getLanguage());
        
        setupSettingsCallbacks();

        // _audioManager.setMasterVolume(_settings.getMasterVolume());
        // _audioManager.setMusicVolume(_settings.getMusicVolume());
        // _audioManager.setSfxVolume(_settings.getSfxVolume());

        _clientNetwork.start();        
        initECS();
        initMenu();
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
        // Enregistrement des composants
        _registry.registerComponent<rtp::ecs::components::ui::Button>();
        _registry.registerComponent<rtp::ecs::components::ui::Text>();
        _registry.registerComponent<rtp::ecs::components::ui::Slider>();
        _registry.registerComponent<rtp::ecs::components::ui::Dropdown>();

        _registry.registerComponent<rtp::ecs::components::Transform>();
        _registry.registerComponent<rtp::ecs::components::Velocity>();
        _registry.registerComponent<rtp::ecs::components::Controllable>();
        _registry.registerComponent<rtp::ecs::components::Sprite>();
        _registry.registerComponent<rtp::ecs::components::Animation>();
        _registry.registerComponent<rtp::ecs::components::NetworkId>();

        _systemManager.addSystem<rtp::client::InputSystem>(_registry, _settings, _clientNetwork, _window);
        _systemManager.addSystem<rtp::client::AnimationSystem>(_registry);
        _systemManager.addSystem<rtp::client::RenderSystem>(_registry, _window);
        
        _systemManager.addSystem<Client::Systems::MenuSystem>(_registry, _window);
        _systemManager.addSystem<Client::Systems::UIRenderSystem>(_registry, _window);
        _systemManager.addSystem<Client::Systems::SettingsMenuSystem>(_registry, _window, _settings);
        
        _systemManager.addSystem<rtp::client::ClientNetworkSystem>(_clientNetwork, _registry);
    }

    void Application::initMenu()
    {
        rtp::log::info("Initializing menu...");
        
        auto titleResult = _registry.spawnEntity();
        if (titleResult) {
            rtp::ecs::Entity title = titleResult.value();
            
            rtp::ecs::components::ui::Text titleText;
            titleText.content = _translations.get("menu.title");
            titleText.position = rtp::Vec2f{UIConstants::TITLE_X, UIConstants::TITLE_Y};
            titleText.fontPath = "assets/fonts/title.ttf";
            titleText.fontSize = static_cast<unsigned int>(UIConstants::TITLE_FONT_SIZE);
            titleText.red = 2;
            titleText.green = 100;
            titleText.blue = 100;
            titleText.zIndex = 10;
            
            _registry.addComponent<rtp::ecs::components::ui::Text>(title, titleText);
        }
        
        auto playBtnResult = _registry.spawnEntity();
        if (playBtnResult) {
            rtp::ecs::Entity playBtn = playBtnResult.value();
            
            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("menu.play");
            button.position = rtp::Vec2f{UIConstants::BUTTON_X, UIConstants::BUTTON_START_Y};
            button.size = rtp::Vec2f{UIConstants::BUTTON_WIDTH, UIConstants::BUTTON_HEIGHT};
            button.onClick = [this]() {
                rtp::log::info("Play button clicked!");
                changeState(GameState::Playing);
            };
            
            _registry.addComponent<rtp::ecs::components::ui::Button>(playBtn, button);
        }
        
        auto settingsBtnResult = _registry.spawnEntity();
        if (settingsBtnResult) {
            rtp::ecs::Entity settingsBtn = settingsBtnResult.value();
            
            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("menu.settings");
            button.position = rtp::Vec2f{UIConstants::BUTTON_X, UIConstants::BUTTON_START_Y + UIConstants::BUTTON_SPACING};
            button.size = rtp::Vec2f{UIConstants::BUTTON_WIDTH, UIConstants::BUTTON_HEIGHT};
            button.onClick = [this]() {
                rtp::log::info("Settings button clicked!");
                changeState(GameState::Settings);
            };
            
            _registry.addComponent<rtp::ecs::components::ui::Button>(settingsBtn, button);
        }
        
        auto exitBtnResult = _registry.spawnEntity();
        if (exitBtnResult) {
            rtp::ecs::Entity exitBtn = exitBtnResult.value();
            
            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("menu.exit");
            button.position = rtp::Vec2f{UIConstants::BUTTON_X, UIConstants::BUTTON_START_Y + UIConstants::BUTTON_SPACING * 2};
            button.size = rtp::Vec2f{UIConstants::BUTTON_WIDTH, UIConstants::BUTTON_HEIGHT};
            button.onClick = [this]() {
                rtp::log::info("Exit button clicked!");
                _window.close();
            };
            
            _registry.addComponent<rtp::ecs::components::ui::Button>(exitBtn, button);
        }
    }

    void Application::initGame()
    {
        rtp::log::info("Starting game...");
        
        // NOTE: Menu entities are not deleted to avoid a crash in killEntity().
        // They remain in the ECS but are not rendered since only the RenderSystem
        // is active during gameplay (UIRenderSystem is only called in Menu state).
        // The menu buttons are still clickable but their callbacks won't do anything harmful.
        // TODO: Fix the killEntity() implementation to properly handle entity deletion,
        // or implement a tag-based filtering system for systems.
        
        // Spawn player
        // auto playerRes = _registry.spawnEntity();
        // if (!playerRes) return;
        // rtp::ecs::Entity p = playerRes.value();

        // _registry.addComponent<rtp::ecs::components::Transform>(
        //     p, rtp::Vec2f{UIConstants::PLAYER_SPAWN_X, UIConstants::PLAYER_SPAWN_Y}, 0.0f, rtp::Vec2f{1.0f, 1.0f});
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

    void Application::initKeyBindingMenu()
    {
        rtp::log::info("Initializing key bindings menu...");

        // Title
        auto titleResult = _registry.spawnEntity();
        if (titleResult) {
            rtp::ecs::Entity title = titleResult.value();
            rtp::ecs::components::ui::Text titleText;
            titleText.content = _translations.get("keybindings.title");
            titleText.position = rtp::Vec2f{450.0f, 50.0f};
            titleText.fontPath = "assets/fonts/main.ttf";
            titleText.fontSize = 60;
            titleText.red = 255;
            titleText.green = 200;
            titleText.blue = 100;
            _registry.addComponent<rtp::ecs::components::ui::Text>(title, titleText);
        }

        float yPos = 150.0f;
        const float buttonSpacing = 60.0f;

        struct KeyBindingButton {
            const char* labelKey;
            KeyAction action;
        };

        std::vector<KeyBindingButton> bindings = {
            {"keybindings.move_up", KeyAction::MoveUp},
            {"keybindings.move_down", KeyAction::MoveDown},
            {"keybindings.move_left", KeyAction::MoveLeft},
            {"keybindings.move_right", KeyAction::MoveRight},
            {"keybindings.shoot", KeyAction::Shoot},
            {"keybindings.pause", KeyAction::Pause}
        };

        for (const auto& binding : bindings) {
            // Label de l'action
            auto labelRes = _registry.spawnEntity();
            if (labelRes) {
                rtp::ecs::Entity label = labelRes.value();
                rtp::ecs::components::ui::Text text;
                text.content = _translations.get(binding.labelKey);
                text.position = rtp::Vec2f{200.0f, yPos + 10.0f};
                text.fontPath = "assets/fonts/main.ttf";
                text.fontSize = 24;
                _registry.addComponent<rtp::ecs::components::ui::Text>(label, text);
            }

            // Bouton avec la touche actuelle
            auto btnRes = _registry.spawnEntity();
            if (btnRes) {
                rtp::ecs::Entity btn = btnRes.value();

                rtp::ecs::components::ui::Button button;
                sf::Keyboard::Key currentKey = _settings.getKey(binding.action);
                button.text = _settings.getKeyName(currentKey);
                button.position = rtp::Vec2f{500.0f, yPos};
                button.size = rtp::Vec2f{250.0f, 50.0f};

                KeyAction actionToBind = binding.action;
                button.onClick = [this, actionToBind]() {
                    rtp::log::info("Waiting for key input for action: {}", 
                                  static_cast<int>(actionToBind));
                    _isWaitingForKey = true;
                    _keyActionToRebind = actionToBind;
                };

                _registry.addComponent<rtp::ecs::components::ui::Button>(btn, button);
            }

            yPos += buttonSpacing;
        }

        // Back Button
        auto backBtnRes = _registry.spawnEntity();
        if (backBtnRes) {
            rtp::ecs::Entity backBtn = backBtnRes.value();

            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("settings.back");
            button.position = rtp::Vec2f{490.0f, 650.0f};
            button.size = rtp::Vec2f{300.0f, 60.0f};
            button.onClick = [this]() {
                rtp::log::info("Back to settings clicked!");
                _settings.save();
                changeState(GameState::Settings);
            };

            _registry.addComponent<rtp::ecs::components::ui::Button>(backBtn, button);
        }
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
            _registry.addComponent<rtp::ecs::components::ui::Text>(title, titleText);
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
            _registry.addComponent<rtp::ecs::components::ui::Slider>(slider, sliderComp);
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
            _registry.addComponent<rtp::ecs::components::ui::Slider>(slider, sliderComp);
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
            _registry.addComponent<rtp::ecs::components::ui::Slider>(slider, sliderComp);
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
            dropdownComp.options = {"English", "Français", "Español", "Deutsch", "Italiano"};
            dropdownComp.selectedIndex = static_cast<int>(_settings.getLanguage());
            dropdownComp.onSelect = [this](int index) {
                _settings.setLanguage(static_cast<Core::Language>(index));
                _translations.loadLanguage(_settings.getLanguage());
                changeState(GameState::Settings);
            };
            _registry.addComponent<rtp::ecs::components::ui::Dropdown>(dropdown, dropdownComp);
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
            dropdownComp.options = {_translations.get("colorblind.none"), _translations.get("colorblind.protanopia"), _translations.get("colorblind.deuteranopia"), _translations.get("colorblind.tritanopia")};
            dropdownComp.selectedIndex = static_cast<int>(_settings.getColorBlindMode());
            dropdownComp.onSelect = [this](int index) {
                _settings.setColorBlindMode(static_cast<Core::ColorBlindMode>(index));
                rtp::log::info("Colorblind mode changed to: {}", index);
            };
            _registry.addComponent<rtp::ecs::components::ui::Dropdown>(dropdown, dropdownComp);
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

            _registry.addComponent<rtp::ecs::components::ui::Button>(btn, button);
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

            _registry.addComponent<rtp::ecs::components::ui::Button>(backBtn, button);
        }
    }

    void Application::changeState(GameState newState)
    {
        rtp::log::info("Changing state from {} to {}", 
                      static_cast<int>(_currentState), 
                      static_cast<int>(newState));

        _registry.clear();
        
        _currentState = newState;
        
        switch (newState) {
            case GameState::Menu:
                initMenu();
                break;
            case GameState::Playing:
                initGame();
                break;
            case GameState::Settings:
                initSettingsMenu();
                break;
            case GameState::KeyBindings:
                initKeyBindingMenu();
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
                // Escape global (sauf en key binding)
                if (kp->code == sf::Keyboard::Key::Escape && _currentState != GameState::KeyBindings) {
                    handleGlobalEscape();
                    return;
                }
                
#ifdef DEBUG
                // Debug keys (spawn/kill) - Only in DEBUG build
                if (kp->code == sf::Keyboard::Key::J) {
                    rtp::Vec2f mousePos{static_cast<float>(sf::Mouse::getPosition(_window).x), 
                                        static_cast<float>(sf::Mouse::getPosition(_window).y)};
                    spawnEnemy(mousePos);
                    rtp::log::debug("[DEBUG] Enemy spawned at ({}, {})", mousePos.x, mousePos.y);
                }
                if (kp->code == sf::Keyboard::Key::K) {
                    killEnemy(0);
                    rtp::log::debug("[DEBUG] Killed enemy at index 0");
                }
                if (kp->code == sf::Keyboard::Key::L) {
                    rtp::Vec2f mousePos{static_cast<float>(sf::Mouse::getPosition(_window).x), 
                                        static_cast<float>(sf::Mouse::getPosition(_window).y)};
                    spawnProjectile(mousePos);
                    rtp::log::debug("[DEBUG] Projectile spawned at ({}, {})", mousePos.x, mousePos.y);
                }
                if (kp->code == sf::Keyboard::Key::M) {
                    killProjectile(0);
                    rtp::log::debug("[DEBUG] Killed projectile at index 0");
                }
#endif
            }

            switch (_currentState) {
                case GameState::Menu:
                    processMenuInput(event.value());
                    break;

                case GameState::Playing:
                    processGameInput(event.value());
                    break;

                case GameState::Settings:
                    processSettingsInput(event.value());
                    break;

                case GameState::KeyBindings:
                    processKeyBindingInput(event.value());
                    break;

                default:
                    break;
            }
        }
    }

    void Application::processMenuInput(const sf::Event& event) {
        // Le MenuSystem gère déjà les clics sur les boutons
        // Rien de plus à faire ici pour l'instant
        (void)event;
    }

    void Application::processGameInput(const sf::Event& event) {
        // Les inputs de debug (J/K/L/M) sont gérés dans processInput()
        // Les touches de mouvement/tir sont gérées par InputSystem
        (void)event;  // Éviter le warning unused
    }

    void Application::processSettingsInput(const sf::Event& event) {
        // Le MenuSystem gère déjà les sliders/dropdowns
        (void)event;
    }

    void Application::processKeyBindingInput(const sf::Event& event) {
        if (!_isWaitingForKey) return;

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

    void Application::handleGlobalEscape() {
        switch (_currentState) {
            case GameState::Menu:
                _window.close();  // Quitter le jeu
                break;

            case GameState::Playing:
                changeState(GameState::Paused);  // Pause (ou Menu)
                break;

            case GameState::Settings:
            case GameState::KeyBindings:
                _settings.save();
                changeState(GameState::Menu);  // Retour au menu
                break;

            case GameState::Paused:
                changeState(GameState::Playing);  // Reprendre le jeu
                break;

            default:
                break;
        }
    }

    void Application::update(sf::Time delta)
    {
        _lastDt = delta.asSeconds();

        if (_currentState == GameState::Menu) {
            auto& menuSys = _systemManager.getSystem<Client::Systems::MenuSystem>();
            menuSys.update(_lastDt);
        } else if (_currentState == GameState::Playing) {
            _systemManager.update(_lastDt);
        } else if (_currentState == GameState::Settings || _currentState == GameState::KeyBindings) {
            auto& menuSys = _systemManager.getSystem<Client::Systems::MenuSystem>();
            menuSys.update(_lastDt);

            auto& settingsSys = _systemManager.getSystem<Client::Systems::SettingsMenuSystem>();
            settingsSys.update(_lastDt);
        }
    }
    
    void Application::setupSettingsCallbacks() {
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

    void Application::spawnProjectile(const rtp::Vec2f& position) {
        std::size_t newIndex = _projectiles.size();
        
        Game::EntityTemplate createBulletEnemy = 
            Game::EntityTemplate::createBulletEnemy(position);

        auto result = _entityBuilder.spawn(createBulletEnemy);

        if (result.has_value()) {
            rtp::ecs::Entity newEnemy = result.value();
            _projectiles.push_back(newEnemy);
            rtp::log::debug("Projectile spawned (ID: {}) at index {}. Total: {} projectiles", 
                           static_cast<std::uint32_t>(newEnemy), newIndex, _projectiles.size());
        } else {
            rtp::log::error("Failed to spawn projectile: {}", result.error().message());
        }
    }

    void Application::spawnEnemy(const rtp::Vec2f& position) {
        std::size_t newIndex = _spawnedEnemy.size();
        
        Game::EntityTemplate scoutTemplate = 
            Game::EntityTemplate::createBasicScout(position);

        auto result = _entityBuilder.spawn(scoutTemplate);

        if (result.has_value()) {
            rtp::ecs::Entity newEnemy = result.value();
            _spawnedEnemy.push_back(newEnemy);
            rtp::log::debug("Enemy spawned (ID: {}) at index {}. Total: {} enemies", 
                           static_cast<std::uint32_t>(newEnemy), newIndex, _spawnedEnemy.size());
        } else {
            rtp::log::error("Failed to spawn enemy: {}", result.error().message());
        }
    }

    void Application::killEnemy(std::size_t index) {
        if (index >= _spawnedEnemy.size()) {
            rtp::log::warning("Cannot kill enemy: index {} out of bounds (0-{})", 
                            index, _spawnedEnemy.empty() ? 0 : _spawnedEnemy.size() - 1);
            return;
        }

        rtp::ecs::Entity entityToKill = _spawnedEnemy.at(index);

        _entityBuilder.kill(entityToKill);
        _spawnedEnemy.erase(_spawnedEnemy.begin() + static_cast<std::vector<rtp::ecs::Entity>::difference_type>(index));

        rtp::log::debug("Enemy killed (ID: {}) at index {}. Remaining: {} enemies", 
                       static_cast<std::uint32_t>(entityToKill), index, _spawnedEnemy.size());
    }

    void Application::killProjectile(std::size_t index) {
        if (index >= _projectiles.size()) {
            rtp::log::warning("Cannot kill projectile: index {} out of bounds (0-{})", 
                            index, _projectiles.empty() ? 0 : _projectiles.size() - 1);
            return;
        }

        rtp::ecs::Entity entityToKill = _projectiles.at(index);

        _entityBuilder.kill(entityToKill);
        _projectiles.erase(_projectiles.begin() + static_cast<std::vector<rtp::ecs::Entity>::difference_type>(index));

        rtp::log::debug("Projectile killed (ID: {}) at index {}. Remaining: {} projectiles", 
                       static_cast<std::uint32_t>(entityToKill), index, _projectiles.size());
    }

    void Application::render()
    {
        _window.clear(sf::Color::Black);
        
        if (_currentState == GameState::Menu || _currentState == GameState::Settings || _currentState == GameState::KeyBindings) {  // ✅ Ajouter
            auto& uiSys = _systemManager.getSystem<Client::Systems::UIRenderSystem>();
            uiSys.update(0.0f);
        } else if (_currentState == GameState::Playing) {
            auto& renderSys = _systemManager.getSystem<rtp::client::RenderSystem>();
            renderSys.update(_lastDt);
        }
        
        _window.display();
    }
}