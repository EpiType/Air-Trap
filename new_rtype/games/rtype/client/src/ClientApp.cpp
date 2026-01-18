/**
 * File   : ClientApp.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/ClientApp.hpp"

#include "engine/log/Logger.hpp"

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

    ClientApp::ClientApp(aer::render::IRenderer& renderer,
                         aer::net::INetwork& network)
        : _renderer(renderer),
          _network(network),
          _worldRegistry(),
          _uiRegistry(),
          _worldFrame(),
          _uiFrame(),
          _settings(),
          _translation(),
          _entityBuilder(_worldRegistry),
          _networkSystem(_network, _worldRegistry, _entityBuilder),
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

    void ClientApp::render(aer::render::RenderFrame& frame)
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

        _scenes.emplace(SceneId::CreateRoom,
                        std::make_unique<scenes::CreateRoomScene>(
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            [this](SceneId s) { setScene(s); }
                        ));

        _scenes.emplace(SceneId::RoomWaiting,
                        std::make_unique<scenes::RoomWaitingScene>(
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            [this](SceneId s) { setScene(s); }
                        ));

        _scenes.emplace(SceneId::Playing,
                        std::make_unique<scenes::PlayingScene>(
                            _worldRegistry,
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            _entityBuilder,
                            [this](SceneId s) { setScene(s); }
                        ));

        _scenes.emplace(SceneId::Paused,
                        std::make_unique<scenes::PauseScene>(
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            [this](SceneId s) { setScene(s); }
                        ));

        _scenes.emplace(SceneId::Settings,
                        std::make_unique<scenes::SettingsScene>(
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            [this](SceneId s) { setScene(s); }
                        ));

        _scenes.emplace(SceneId::KeyBindings,
                        std::make_unique<scenes::KeyBindingScene>(
                            _uiRegistry,
                            _settings,
                            _translation,
                            _networkSystem,
                            [this](SceneId s) { setScene(s); }
                        ));

        _scenes.emplace(SceneId::GamepadSettings,
                        std::make_unique<scenes::GamepadSettingsScene>(
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

        register_world.template operator()<aer::ecs::components::Transform>();
        register_world.template operator()<aer::ecs::components::Velocity>();
        register_world.template operator()<aer::ecs::components::Sprite>();
        register_world.template operator()<aer::ecs::components::Animation>();
        register_world.template operator()<aer::ecs::components::NetworkId>();
        register_world.template operator()<aer::ecs::components::BoundingBox>();
        register_world.template operator()<aer::ecs::components::Hitbox>();
        register_world.template operator()<rtp::ecs::components::ParallaxLayer>();

        register_ui.template operator()<aer::ecs::components::Button>();
        register_ui.template operator()<aer::ecs::components::Text>();
        register_ui.template operator()<aer::ecs::components::Slider>();
        register_ui.template operator()<aer::ecs::components::Dropdown>();
        register_ui.template operator()<aer::ecs::components::TextInput>();
    }

    void ClientApp::handleEvents(const aer::input::Event& event)
    {
        _uiSystem.handleEvent(event);
        if (auto it = _scenes.find(_activeScene); it != _scenes.end()) {
            it->second->handleEvent(event);
        }
    }
} // namespace rtp::client

extern "C"
{
    RTYPE_CLIENT_API rtp::client::ClientApp *CreateClientApp(
        aer::render::IRenderer *renderer,
        aer::net::INetwork *network)
    {
        if (!renderer || !network) return nullptr;
        return new rtp::client::ClientApp(*renderer, *network);
    }

    RTYPE_CLIENT_API void DestroyClientApp(rtp::client::ClientApp *app)
    {
        delete app;
    }

    RTYPE_CLIENT_API void ClientAppInit(rtp::client::ClientApp *app)
    {
        if (app) {
            app->init();
        }
    }

    RTYPE_CLIENT_API void ClientAppShutdown(rtp::client::ClientApp *app)
    {
        if (app) {
            app->shutdown();
        }
    }

    RTYPE_CLIENT_API void ClientAppUpdate(rtp::client::ClientApp *app, float dt)
    {
        if (app) {
            app->update(dt);
        }
    }

    RTYPE_CLIENT_API void ClientAppRender(rtp::client::ClientApp *app,
                                          aer::render::RenderFrame *frame)
    {
        if (app && frame) {
            app->render(*frame);
        }
    }

    RTYPE_CLIENT_API void ClientAppHandleEvent(rtp::client::ClientApp *app,
                                               const aer::input::Event *event)
    {
        if (app && event) {
            app->handleEvents(*event);
        }
    }
}
