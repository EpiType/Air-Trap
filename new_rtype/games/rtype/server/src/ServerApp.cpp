/**
 * File   : ServerApp.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/ServerApp.hpp"

#include "engine/log/Logger.hpp"

#include <utility>

namespace rtp::server
{
    namespace
    {
        using CreateNetworkEngineFn = aer::net::INetworkEngine* (*)();
        using DestroyNetworkEngineFn = void (*)(aer::net::INetworkEngine*);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    ServerApp::ServerApp(Config config)
        : _config(std::move(config))
    {
    }

    bool ServerApp::init(void)
    {
        if (!loadNetworkEngine()) {
            return false;
        }

        aer::net::ServerConfig netConfig{};
        netConfig.bindAddress = _config.bindAddress;
        netConfig.tcpPort = _config.tcpPort;
        netConfig.udpPort = _config.udpPort;
        netConfig.maxSessions = _config.maxSessions;

        _network = std::make_unique<net::NetworkBootstrap>(*_networkEngine, netConfig);
        _gameManager = std::make_unique<GameManager>(*_network);
        return true;
    }

    void ServerApp::run(void)
    {
        if (!_network || !_gameManager) {
            return;
        }
        if (!_network->start()) {
            aer::log::error("Server: failed to start network");
            return;
        }

        _running = true;
        using clock = std::chrono::steady_clock;
        const auto step = std::chrono::duration<double>(1.0 / _config.tickRate);
        auto last = clock::now();
        double accumulator = 0.0;

        while (_running) {
            auto now = clock::now();
            const double dt = std::chrono::duration<double>(now - last).count();
            last = now;
            accumulator += dt;

            while (auto event = _network->pollEvent()) {
                _gameManager->handleEvent(*event);
            }

            while (accumulator >= step.count()) {
                _gameManager->update(static_cast<float>(step.count()));
                accumulator -= step.count();
            }
        }

        _network->stop();
    }

    void ServerApp::stop(void)
    {
        _running = false;
    }

    void ServerApp::shutdown(void)
    {
        if (_network) {
            _network->stop();
        }
        _gameManager.reset();
        _network.reset();

        if (_networkLib && _networkEngine) {
            auto destroy = _networkLib->get<DestroyNetworkEngineFn>("DestroyNetworkEngine");
            if (destroy) {
                destroy.value()(_networkEngine);
            }
        }
        _networkEngine = nullptr;
        _networkLib.reset();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    bool ServerApp::loadNetworkEngine(void)
    {
        if (_config.networkPluginPath.empty()) {
            aer::log::error("Server: network plugin path is empty");
            return false;
        }

        auto libRes = _libraries.loadShared(_config.networkPluginPath);
        if (!libRes) {
            aer::log::error("Server: failed to load network plugin: {}",
                                _config.networkPluginPath);
            return false;
        }
        _networkLib = libRes.value();

        auto create = _networkLib->get<CreateNetworkEngineFn>("CreateNetworkEngine");
        if (!create) {
            aer::log::error("Server: CreateNetworkEngine not found");
            return false;
        }

        _networkEngine = create.value()();
        if (!_networkEngine) {
            aer::log::error("Server: CreateNetworkEngine failed");
            return false;
        }

        return true;
    }
}

extern "C"
{
    RTYPE_SERVER_API rtp::server::ServerApp *CreateServerApp(
        rtp::server::ServerApp::Config config)
    {
        return new rtp::server::ServerApp(std::move(config));
    }

    RTYPE_SERVER_API void DestroyServerApp(rtp::server::ServerApp *app)
    {
        delete app;
    }

    RTYPE_SERVER_API rtp::server::ServerApp *CreateServerAppWithConfig(
        const char *networkPluginPath,
        const char *bindAddress,
        std::uint16_t tcpPort,
        std::uint16_t udpPort,
        std::uint32_t maxSessions,
        double tickRate)
    {
        rtp::server::ServerApp::Config config{};
        if (networkPluginPath) {
            config.networkPluginPath = networkPluginPath;
        }
        if (bindAddress) {
            config.bindAddress = bindAddress;
        }
        if (tcpPort != 0) {
            config.tcpPort = tcpPort;
        }
        if (udpPort != 0) {
            config.udpPort = udpPort;
        }
        if (maxSessions != 0) {
            config.maxSessions = maxSessions;
        }
        if (tickRate > 0.0) {
            config.tickRate = tickRate;
        }
        return new rtp::server::ServerApp(std::move(config));
    }

    RTYPE_SERVER_API bool ServerAppInit(rtp::server::ServerApp *app)
    {
        return app ? app->init() : false;
    }

    RTYPE_SERVER_API void ServerAppRun(rtp::server::ServerApp *app)
    {
        if (app) {
            app->run();
        }
    }

    RTYPE_SERVER_API void ServerAppStop(rtp::server::ServerApp *app)
    {
        if (app) {
            app->stop();
        }
    }

    RTYPE_SERVER_API void ServerAppShutdown(rtp::server::ServerApp *app)
    {
        if (app) {
            app->shutdown();
        }
    }
}
