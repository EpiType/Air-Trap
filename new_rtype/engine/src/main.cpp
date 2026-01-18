/**
 * File   : main.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "engine/core/Core.hpp"
#include "engine/input/Event.hpp"
#include "engine/log/Logger.hpp"
#include "engine/net/INetwork.hpp"
#include "engine/net/INetworkEngine.hpp"
#include "engine/plugin/DynamicLibrary.hpp"

#include <cstdlib>
#include <string>

namespace rtp::client { class ClientApp; }
namespace rtp::server { class ServerApp; }

namespace
{
    using CreateClientAppFn =
        rtp::client::ClientApp *(*)(aer::render::IRenderer *, aer::net::INetwork *);
    using DestroyClientAppFn =
        void (*)(rtp::client::ClientApp *);
    using ClientInitFn =
        void (*)(rtp::client::ClientApp *);
    using ClientShutdownFn =
        void (*)(rtp::client::ClientApp *);
    using ClientUpdateFn =
        void (*)(rtp::client::ClientApp *, float);
    using ClientRenderFn =
        void (*)(rtp::client::ClientApp *, aer::render::RenderFrame *);
    using ClientEventFn =
        void (*)(rtp::client::ClientApp *, const aer::input::Event *);

    using CreateServerAppFn =
        rtp::server::ServerApp *(*)(const char *, const char *,
                                    std::uint16_t, std::uint16_t,
                                    std::uint32_t, double);
    using DestroyServerAppFn =
        void (*)(rtp::server::ServerApp *);
    using ServerInitFn =
        bool (*)(rtp::server::ServerApp *);
    using ServerRunFn =
        void (*)(rtp::server::ServerApp *);
    using ServerStopFn =
        void (*)(rtp::server::ServerApp *);
    using ServerShutdownFn =
        void (*)(rtp::server::ServerApp *);

    template <typename Fn>
    bool loadSymbol(const std::shared_ptr<aer::plugin::DynamicLibrary> &lib,
                    std::string_view name,
                    Fn &out)
    {
        auto res = lib->get<Fn>(name);
        if (!res) {
            aer::log::error("Engine: missing symbol '{}'", name);
            return false;
        }
        out = res.value();
        return true;
    }
}

int main(int argc, char **argv)
{
    bool runServer = false;
    bool runClient = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i] ? argv[i] : "";
        if (arg == "--server") {
            runServer = true;
        } else if (arg == "--client") {
            runClient = true;
        }
    }
    if (!runServer && !runClient) {
        runClient = true;
    }

    try {
        auto &core = aer::core::Core::instance();
        aer::core::Core::Config config{};
        config.headless = runServer;

        if (!core.init(config)) {
            aer::log::error("Engine: failed to init core");
            return EXIT_FAILURE;
        }

        const auto &coreConfig = core.config();
        if (runServer) {
            if (coreConfig.defaultGameServerPath.empty()) {
                aer::log::error("Engine: default game server path is empty");
                return EXIT_FAILURE;
            }

            auto gameRes = core.loadLibrary(coreConfig.defaultGameServerPath);
            if (!gameRes) {
                aer::log::error("Engine: failed to load game server: {}",
                                coreConfig.defaultGameServerPath);
                return EXIT_FAILURE;
            }
            auto gameLib = gameRes.value();

            CreateServerAppFn createApp = nullptr;
            DestroyServerAppFn destroyApp = nullptr;
            ServerInitFn initApp = nullptr;
            ServerRunFn runApp = nullptr;
            ServerStopFn stopApp = nullptr;
            ServerShutdownFn shutdownApp = nullptr;

            if (!loadSymbol(gameLib, "CreateServerAppWithConfig", createApp)
                || !loadSymbol(gameLib, "DestroyServerApp", destroyApp)
                || !loadSymbol(gameLib, "ServerAppInit", initApp)
                || !loadSymbol(gameLib, "ServerAppRun", runApp)
                || !loadSymbol(gameLib, "ServerAppStop", stopApp)
                || !loadSymbol(gameLib, "ServerAppShutdown", shutdownApp)) {
                return EXIT_FAILURE;
            }

            const std::string bindAddress =
                coreConfig.networkIp.empty() ? "0.0.0.0" : coreConfig.networkIp;
            const std::uint16_t tcpPort =
                (coreConfig.networkPort == 0) ? 4242 : coreConfig.networkPort;
            const std::uint16_t udpPort =
                (coreConfig.networkPort == 0) ? 4243 : static_cast<std::uint16_t>(coreConfig.networkPort + 1);

            auto *app = createApp(coreConfig.networkPluginPath.c_str(),
                                  bindAddress.c_str(),
                                  tcpPort, udpPort, 64, 60.0);
            if (!app) {
                aer::log::error("Engine: failed to create game server");
                return EXIT_FAILURE;
            }
            if (!initApp(app)) {
                aer::log::error("Engine: failed to init game server");
                destroyApp(app);
                return EXIT_FAILURE;
            }
            runApp(app);
            stopApp(app);
            shutdownApp(app);
            destroyApp(app);
            core.shutdown();
            return EXIT_SUCCESS;
        }

        if (coreConfig.networkPluginPath.empty()) {
            aer::log::error("Engine: network plugin path is empty");
            return EXIT_FAILURE;
        }
        if (!core.loadNetworkPlugin(coreConfig.networkPluginPath)) {
            aer::log::error("Engine: failed to load network plugin: {}",
                            coreConfig.networkPluginPath);
            return EXIT_FAILURE;
        }

        auto *renderer = core.getCurrentRendereEngine();
        auto *networkEngine = core.getCurrentNetworkEngine();
        if (!renderer || !networkEngine) {
            aer::log::error("Engine: missing renderer or network engine");
            return EXIT_FAILURE;
        }

        aer::net::ClientConfig netConfig{};
        netConfig.host = coreConfig.networkIp.empty()
            ? "127.0.0.1"
            : coreConfig.networkIp;
        const std::uint16_t port =
            (coreConfig.networkPort == 0) ? 4242 : coreConfig.networkPort;
        netConfig.tcpPort = port;
        netConfig.udpPort = port;

        auto *network = networkEngine->createClient(netConfig);
        if (!network || !network->start()) {
            aer::log::error("Engine: failed to start network");
            return EXIT_FAILURE;
        }

        if (coreConfig.defaultGameClientPath.empty()) {
            aer::log::error("Engine: default game client path is empty");
            return EXIT_FAILURE;
        }

        auto gameRes = core.loadLibrary(coreConfig.defaultGameClientPath);
        if (!gameRes) {
            aer::log::error("Engine: failed to load game client: {}",
                            coreConfig.defaultGameClientPath);
            return EXIT_FAILURE;
        }
        auto gameLib = gameRes.value();

        CreateClientAppFn createApp = nullptr;
        DestroyClientAppFn destroyApp = nullptr;
        ClientInitFn initApp = nullptr;
        ClientShutdownFn shutdownApp = nullptr;
        ClientUpdateFn updateApp = nullptr;
        ClientRenderFn renderApp = nullptr;
        ClientEventFn eventApp = nullptr;

        if (!loadSymbol(gameLib, "CreateClientApp", createApp)
            || !loadSymbol(gameLib, "DestroyClientApp", destroyApp)
            || !loadSymbol(gameLib, "ClientAppInit", initApp)
            || !loadSymbol(gameLib, "ClientAppShutdown", shutdownApp)
            || !loadSymbol(gameLib, "ClientAppUpdate", updateApp)
            || !loadSymbol(gameLib, "ClientAppRender", renderApp)
            || !loadSymbol(gameLib, "ClientAppHandleEvent", eventApp)) {
            return EXIT_FAILURE;
        }

        auto *app = createApp(renderer, network);
        if (!app) {
            aer::log::error("Engine: failed to create game client");
            return EXIT_FAILURE;
        }

        initApp(app);

        core.setUpdateCallback([&](float dt) { updateApp(app, dt); });
        core.setRenderCallback([&](aer::render::RenderFrame &frame) {
            renderApp(app, &frame);
        });
        core.setEventCallback([&](const aer::input::Event &event) {
            eventApp(app, &event);
        });
        core.run();

        shutdownApp(app);
        destroyApp(app);
        network->stop();
        networkEngine->destroy(network);
        core.shutdown();
        return EXIT_SUCCESS;
    } catch (const std::exception &err) {
        aer::log::error("Engine: exception: {}", err.what());
    }

    return EXIT_FAILURE;
}
