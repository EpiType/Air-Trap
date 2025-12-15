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
#include "Systems/MenuSystem.hpp"
#include "Systems/UIRenderSystem.hpp"
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
    }

    void Application::initMenu()
    {
        rtp::log::info("Initializing menu...");
        
        // ✅ Create title text
        auto titleResult = _registry.spawnEntity();
        if (titleResult) {
            rtp::ecs::Entity title = titleResult.value();
            
            rtp::ecs::components::ui::Text titleText;
            titleText.content = "AIR-TRAP";
            titleText.position = rtp::Vec2f{400.0f, 100.0f};
            titleText.fontPath = "assets/fonts/main.ttf";
            titleText.fontSize = 72;
            titleText.red = 255;
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
            button.text = "PLAY";
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
            button.text = "SETTINGS";
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
            button.text = "EXIT";
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

    void Application::changeState(GameState newState)
    {
        rtp::log::info("Changing state from {} to {}", 
                      static_cast<int>(_currentState), 
                      static_cast<int>(newState));
        
        _currentState = newState;
        
        switch (newState) {
            case GameState::Playing:
                initGame();
                break;
            case GameState::Settings:
                // TODO: Init settings menu
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
            // Menu systems handled by SystemManager
            auto& menuSys = _systemManager.getSystem<Client::Systems::MenuSystem>();
            menuSys.update(_lastDt);
        } else if (_currentState == GameState::Playing) {
            _systemManager.update(_lastDt);
        }
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
        }
        
        _window.display();
    }
}