/**
 * File   : main.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "engine/core/Core.hpp"
#include "engine/core/Error.hpp"
#include "engine/core/Logger.hpp"
#include "rtype/ClientApp.hpp"
#include "rtype/net/NetworkBootstrap.hpp"

#include <cstdlib>
#include <string>

int main(int argc, char **argv)
{
    try {
        const std::string rendererOverride =
            ((argc > 1) && argv[1] && *argv[1]) ? argv[1] : "";
        const std::string networkOverride =
            ((argc > 2) && argv[2] && *argv[2]) ? argv[2] : "";

        auto &core = engine::core::Core::instance();
        engine::core::Core::Config config{};
        config.title = "RType";
        config.rendererPluginPath = rendererOverride;
        config.networkPluginPath = networkOverride;

        if (!core.init(config)) {
            engine::core::error("Client: failed to init core");
            return EXIT_FAILURE;
        }
        const auto &coreConfig = core.config();
        const std::string networkPath =
            !networkOverride.empty() ? networkOverride : coreConfig.networkPluginPath;
        if (!core.loadNetworkPlugin(networkPath)) {
            engine::core::error("Client: failed to load network plugin: {}", networkPath);
            return EXIT_FAILURE;
        }

        auto *renderer = core.getCurrentRendereEngine();
        auto *networkEngine = core.getCurrentNetworkEngine();
        if (!renderer || !networkEngine) {
            engine::core::error("Client: core missing renderer or network engine");
            return EXIT_FAILURE;
        }

        engine::net::ClientConfig netConfig{};
        netConfig.host = coreConfig.networkIp.empty()
            ? "127.0.0.1"
            : coreConfig.networkIp;
        const std::uint16_t port =
            (coreConfig.networkPort == 0) ? 4242 : coreConfig.networkPort;
        netConfig.tcpPort = port;
        netConfig.udpPort = port;

        rtp::client::NetworkBootstrap network(*networkEngine, netConfig);
        if (!network.start()) {
            engine::core::error("Client: failed to start network");
            return EXIT_FAILURE;
        }

        rtp::client::ClientApp app(*renderer, network);
        app.init();

        core.setUpdateCallback([&](float dt) { app.update(dt); });
        core.setRenderCallback([&](engine::render::RenderFrame &frame) {
            app.render(frame);
        });
        core.setEventCallback([&](const engine::input::Event &event) {
            app.handleEvents(event);
        });
        core.run();

        app.shutdown();
        network.stop();
        core.shutdown();
        return EXIT_SUCCESS;
    } catch (const engine::core::Error &err) {
        engine::core::error("Client: exception: {}", err);
    } catch (const std::exception &err) {
        engine::core::error("Client: exception: {}", err.what());
    }

    return EXIT_FAILURE;
}
