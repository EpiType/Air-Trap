/**
 * File   : main.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "engine/log/Logger.hpp"
#include "rtype/ServerApp.hpp"

#include <cstdlib>
#include <filesystem>
#include <string>

namespace
{
    std::string defaultNetworkPath()
    {
#if defined(_WIN32)
        return "plugins/net_asio.dll";
#elif defined(__APPLE__)
        return "plugins/libnet_asio.dylib";
#else
        return "plugins/libnet_asio.so";
#endif
    }

    std::string resolvePath(const char *argPath, const std::string &fallback)
    {
        if (argPath && *argPath != '\0') {
            return argPath;
        }
        if (std::filesystem::exists(fallback)) {
            return fallback;
        }
        return (std::filesystem::path("..") / fallback).string();
    }
}

int main(int argc, char **argv)
{
    rtp::server::ServerApp::Config config{};
    config.networkPluginPath = resolvePath((argc > 1) ? argv[1] : nullptr,
                                           defaultNetworkPath());

    rtp::server::ServerApp server(config);
    if (!server.init()) {
        aer::log::error("Server: init failed");
        return EXIT_FAILURE;
    }

    server.run();
    server.shutdown();
    return EXIT_SUCCESS;
}
