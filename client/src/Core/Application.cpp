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

// Systems
#include "Systems/InputSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/AnimationSystem.hpp"
#include "Systems/RenderSystem.hpp"

#include "RType/Math/Vec2.hpp"

namespace Client::Core
{
    Application::Application()
        : _window(sf::VideoMode({1280, 720}), "Air-Trap - R-Type Clone")
        , _systemManager(_registry)
    {
        _window.setFramerateLimit(60);
        _systemManager.setWindow(_window);
        _settings.load();
        
        // ✅ Setup des callbacks pour changements en temps réel
        setupSettingsCallbacks();
        
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
        // Register UI components
        _registry.registerComponent<rtp::ecs::components::ui::Button>();
        _registry.registerComponent<rtp::ecs::components::ui::Text>();
        _registry.registerComponent<rtp::ecs::components::ui::Slider>();
        _registry.registerComponent<rtp::ecs::components::ui::Dropdown>();
        
        _registry.registerComponent<rtp::ecs::components::Transform>();
        _registry.registerComponent<rtp::ecs::components::Velocity>();
        _registry.registerComponent<rtp::ecs::components::Controllable>();
        _registry.registerComponent<rtp::ecs::components::Sprite>();
        _registry.registerComponent<rtp::ecs::components::Animation>();

        // Add systems to SystemManager - window injected automatically!
        _systemManager.addSystem<rtp::client::InputSystem>();
        _systemManager.addSystem<rtp::client::MovementSystem>();
        _systemManager.addSystem<rtp::client::AnimationSystem>();
        _systemManager.addSystem<rtp::client::RenderSystem>();
        _systemManager.addSystem<Client::Systems::MenuSystem>();
        _systemManager.addSystem<Client::Systems::UIRenderSystem>();
        _systemManager.addSystem<Client::Systems::SettingsMenuSystem>(_settings);
    }

    void Application::initMenu()
    {
        rtp::log::info("Initializing menu...");
        
        // ✅ Create title text
        auto titleResult = _registry.spawnEntity();
        if (titleResult) {
            rtp::ecs::Entity title = titleResult.value();
            
            rtp::ecs::components::ui::Text titleText;
            titleText.content = _translations.get("menu.title");
            titleText.position = rtp::Vec2f{400.0f, 100.0f};
            titleText.fontPath = "assets/fonts/title.otf";
            titleText.fontSize = 72;
            titleText.red = 2;
            titleText.green = 100;
            titleText.blue = 100;
            titleText.zIndex = 10;
            
            _registry.addComponent<rtp::ecs::components::ui::Text>(title, titleText);
        }
        
        // ✅ Create Play button
        auto playBtnResult = _registry.spawnEntity();
        if (playBtnResult) {
            rtp::ecs::Entity playBtn = playBtnResult.value();
            
            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("menu.play");
            button.position = rtp::Vec2f{490.0f, 300.0f};
            button.size = rtp::Vec2f{300.0f, 60.0f};
            button.onClick = [this]() {
                rtp::log::info("Play button clicked!");
                changeState(GameState::Playing);
            };
            
            _registry.addComponent<rtp::ecs::components::ui::Button>(playBtn, button);
        }
        
        // ✅ Create Settings button
        auto settingsBtnResult = _registry.spawnEntity();
        if (settingsBtnResult) {
            rtp::ecs::Entity settingsBtn = settingsBtnResult.value();
            
            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("menu.settings");
            button.position = rtp::Vec2f{490.0f, 380.0f};
            button.size = rtp::Vec2f{300.0f, 60.0f};
            button.onClick = [this]() {
                rtp::log::info("Settings button clicked!");
                changeState(GameState::Settings);
            };
            
            _registry.addComponent<rtp::ecs::components::ui::Button>(settingsBtn, button);
        }
        
        // ✅ Create Exit button
        auto exitBtnResult = _registry.spawnEntity();
        if (exitBtnResult) {
            rtp::ecs::Entity exitBtn = exitBtnResult.value();
            
            rtp::ecs::components::ui::Button button;
            button.text = _translations.get("menu.exit");
            button.position = rtp::Vec2f{490.0f, 460.0f};
            button.size = rtp::Vec2f{300.0f, 60.0f};
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
        auto playerRes = _registry.spawnEntity();
        if (!playerRes) return;
        rtp::ecs::Entity p = playerRes.value();

        _registry.addComponent<rtp::ecs::components::Transform>(
            p, rtp::Vec2f{200.f, 150.f}, 0.0f, rtp::Vec2f{1.0f, 1.0f});
        _registry.addComponent<rtp::ecs::components::Velocity>(p);
        _registry.addComponent<rtp::ecs::components::Controllable>(p);

        rtp::ecs::components::Sprite spriteData;
        spriteData.texturePath = "assets/sprites/r-typesheet42.gif";
        spriteData.rectLeft = 0;
        spriteData.rectTop = 0;
        spriteData.rectWidth = 33;
        spriteData.rectHeight = 17;
        spriteData.zIndex = 10;
        spriteData.red = 255;

        _registry.addComponent<rtp::ecs::components::Sprite>(p, spriteData);

        rtp::ecs::components::Animation animData;
        animData.frameLeft = 0;
        animData.frameTop = 0;
        animData.frameWidth = 33;
        animData.frameHeight = 17;
        animData.totalFrames = 5;
        animData.frameDuration = 0.1f;
        animData.currentFrame = 0;
        animData.elapsedTime = 0.0f;

        _registry.addComponent<rtp::ecs::components::Animation>(p, animData);
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
            };
            _registry.addComponent<rtp::ecs::components::ui::Dropdown>(dropdown, dropdownComp);
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
                if (kp->code == sf::Keyboard::Key::Escape)
                    _window.close();
            }
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
        } else if (_currentState == GameState::Settings) {
            auto& menuSys = _systemManager.getSystem<Client::Systems::MenuSystem>();
            menuSys.update(_lastDt);

            auto& settingsSys = _systemManager.getSystem<Client::Systems::SettingsMenuSystem>();
            settingsSys.update(_lastDt);
        }
    }
    
    void Application::setupSettingsCallbacks() {
        // ✅ Volume Master - appliqué instantanément
        _settings.onMasterVolumeChanged([this](float volume) {
            rtp::log::info("Master volume changed to: {:.2f}", volume);
            // TODO: Appliquer au AudioManager quand il sera implémenté
        });
        
        // ✅ Volume Musique - appliqué instantanément
        _settings.onMusicVolumeChanged([this](float volume) {
            rtp::log::info("Music volume changed to: {:.2f}", volume);
            // TODO: Appliquer au AudioManager quand il sera implémenté
        });
        
        // ✅ Volume SFX - appliqué instantanément
        _settings.onSfxVolumeChanged([this](float volume) {
            rtp::log::info("SFX volume changed to: {:.2f}", volume);
            // TODO: Appliquer au AudioManager quand il sera implémenté
        });
        
        // ✅ Changement de langue - recharge l'UI
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

    void Application::render()
    {
        _window.clear(sf::Color::Black);
        
        if (_currentState == GameState::Menu) {
            auto& uiSys = _systemManager.getSystem<Client::Systems::UIRenderSystem>();
            uiSys.update(0.0f);
        } else if (_currentState == GameState::Playing) {
            auto& renderSys = _systemManager.getSystem<rtp::client::RenderSystem>();
            renderSys.update(_lastDt);
        } else if (_currentState == GameState::Settings) {
            auto& uiSys = _systemManager.getSystem<Client::Systems::UIRenderSystem>();
            uiSys.update(0.0f);
        }
        
        _window.display();
    }
}