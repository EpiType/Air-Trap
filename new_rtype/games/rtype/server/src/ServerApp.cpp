/**
 * File   : ServerApp.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/ServerApp.hpp"

#include "engine/core/Logger.hpp"

namespace rtp::server
{
    namespace
    {
        using CreateNetworkEngineFn = engine::net::INetworkEngine* (*)();
        using DestroyNetworkEngineFn = void (*)(engine::net::INetworkEngine*);
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

        engine::net::ServerConfig netConfig{};
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
            engine::core::error("Server: failed to start network");
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
            engine::core::error("Server: network plugin path is empty");
            return false;
        }

        auto libRes = _libraries.loadShared(_config.networkPluginPath);
        if (!libRes) {
            engine::core::error("Server: failed to load network plugin: {}",
                                _config.networkPluginPath);
            return false;
        }
        _networkLib = libRes.value();

        auto create = _networkLib->get<CreateNetworkEngineFn>("CreateNetworkEngine");
        if (!create) {
            engine::core::error("Server: CreateNetworkEngine not found");
            return false;
        }

        _networkEngine = create.value()();
        if (!_networkEngine) {
            engine::core::error("Server: CreateNetworkEngine failed");
            return false;
        }

        return true;
    }
}
