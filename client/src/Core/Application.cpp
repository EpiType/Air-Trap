/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
** Application.cpp, main application loop
*/

#include "Core/Application.hpp"

#include <memory>

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Clock.hpp>

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
        initECS();
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
        _registry.registerComponent<rtp::ecs::components::Transform>();
        _registry.registerComponent<rtp::ecs::components::Velocity>();
        _registry.registerComponent<rtp::ecs::components::Controllable>();
        _registry.registerComponent<rtp::ecs::components::Sprite>();
        _registry.registerComponent<rtp::ecs::components::Animation>();

        _systemManager.addSystem<rtp::client::InputSystem>(_registry);
        _systemManager.addSystem<rtp::client::MovementSystem>(_registry);
        _systemManager.addSystem<rtp::client::AnimationSystem>(_registry);

        _renderSystem = std::make_unique<rtp::client::RenderSystem>(_registry, _window);

        // Spawn player (comme ton main.cpp)
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
        _systemManager.update(_lastDt);
    }

    void Application::render()
    {
        if (_renderSystem)
            _renderSystem->update(_lastDt);
    }
}