/**
 * File   : ClientApp.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/ClientApp.hpp"

#include "engine/core/Logger.hpp"

#include <chrono>

#include "engine/ecs/components/ImportAllEngineComponents.hpp"
#include "engine/ecs/components/ui/Button.hpp"
#include "engine/ecs/components/ui/Dropdown.hpp"
#include "engine/ecs/components/ui/Slider.hpp"
#include "engine/ecs/components/ui/Text.hpp"
#include "engine/ecs/components/ui/TextInput.hpp"
#include "rtype/components/ParallaxLayer.hpp"

namespace rtp::client
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    ClientApp::ClientApp(engine::render::IRenderer& renderer,
                         engine::net::INetwork& network)
        : _renderer(renderer),
          _network(network),
          _worldRegistry(),
          _uiRegistry(),
          _worldFrame(),
          _uiFrame(),
          _settings(),
          _translation(),
          _networkSystem(_network, _worldRegistry, EntityBuilder(_worldRegistry)),
          _inputSystem(_worldRegistry, _uiRegistry, _settings, _network, 0),
          _renderSystem(_worldRegistry, _renderer, _worldFrame),
          _uiRenderSystem(_uiRegistry, _renderer, _uiFrame),
          _uiSystem(_uiRegistry, _renderer, _settings),
          _parallaxSystem(_worldRegistry),
          _animationSystem(_worldRegistry)
    {
    }

    void ClientApp::init(void)
    {
        registerComponents();
        _translation.loadLanguage(_settings.language());
        createScenes();
        setScene(SceneId::Login);
    }

    void ClientApp::shutdown(void)
    {
        _scenes.clear();
        _uiRegistry.clear();
        _worldRegistry.clear();
    }

    void ClientApp::update(float dt)
    {
        _networkSystem.update(dt);
        _inputSystem.update(dt);
        _parallaxSystem.update(dt);
        _animationSystem.update(dt);

        if (auto it = _scenes.find(_activeScene); it != _scenes.end()) {
            it->second->update(dt);
        }

        _uiSystem.update(dt);
    }

    void ClientApp::render(engine::render::RenderFrame& frame)
    {
        _renderSystem.update(0.0f);
        _uiRenderSystem.update(0.0f);

        frame.sprites.insert(frame.sprites.end(),
                             _worldFrame.sprites.begin(),
                             _worldFrame.sprites.end());
        frame.texts.insert(frame.texts.end(),
                           _worldFrame.texts.begin(),
                           _worldFrame.texts.end());
        frame.shapes.insert(frame.shapes.end(),
                            _worldFrame.shapes.begin(),
                            _worldFrame.shapes.end());

        frame.sprites.insert(frame.sprites.end(),
                             _uiFrame.sprites.begin(),
                             _uiFrame.sprites.end());
        frame.texts.insert(frame.texts.end(),
                           _uiFrame.texts.begin(),
                           _uiFrame.texts.end());
        frame.shapes.insert(frame.shapes.end(),
                            _uiFrame.shapes.begin(),
                            _uiFrame.shapes.end());

    }

    void ClientApp::setScene(SceneId sceneId)
    {
        if (_sceneActive && _activeScene == sceneId && !_scenes.empty()) {
            return;
        }
        if (auto it = _scenes.find(_activeScene); it != _scenes.end()) {
            it->second->onExit();
        }
        _activeScene = sceneId;
        if (auto it = _scenes.find(_activeScene); it != _scenes.end()) {
            it->second->onEnter();
            _sceneActive = true;
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void ClientApp::createScenes(void)
    {
        _scenes.emplace(SceneId::Login,
                        std::make_unique<scenes::LoginScene>(
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            [this](SceneId s) { setScene(s); }
                        ));
                        
        _scenes.emplace(SceneId::Menu,
                        std::make_unique<scenes::MenuScene>(
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            [this](SceneId s) { setScene(s); }
                        ));

        _scenes.emplace(SceneId::Lobby,
                        std::make_unique<scenes::LobbyScene>(
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            [this](SceneId s) { setScene(s); }
                        ));
    }

    void ClientApp::registerComponents(void)
    {
        auto register_world = [this]<typename T>() {
            (void)_worldRegistry.registerComponent<T>();
        };
        auto register_ui = [this]<typename T>() {
            (void)_uiRegistry.registerComponent<T>();
        };

        register_world.template operator()<engine::ecs::components::Transform>();
        register_world.template operator()<engine::ecs::components::Velocity>();
        register_world.template operator()<engine::ecs::components::Sprite>();
        register_world.template operator()<engine::ecs::components::Animation>();
        register_world.template operator()<engine::ecs::components::NetworkId>();
        register_world.template operator()<engine::ecs::components::BoundingBox>();
        register_world.template operator()<engine::ecs::components::Hitbox>();
        register_world.template operator()<rtp::ecs::components::ParallaxLayer>();

        register_ui.template operator()<engine::ecs::components::Button>();
        register_ui.template operator()<engine::ecs::components::Text>();
        register_ui.template operator()<engine::ecs::components::Slider>();
        register_ui.template operator()<engine::ecs::components::Dropdown>();
        register_ui.template operator()<engine::ecs::components::TextInput>();
    }

    void ClientApp::handleEvents(const engine::input::Event& event)
    {
        _uiSystem.handleEvent(event);
        if (auto it = _scenes.find(_activeScene); it != _scenes.end()) {
            it->second->handleEvent(event);
        }
    }
} // namespace rtp::client
