/**
 * File   : main.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN
 * <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 10/12/2025
 */

#include "Game/GameManager.hpp"
#include "RType/Logger.hpp"
#include "ServerNetwork/ServerNetwork.hpp"

#include <chrono>
#include <csignal>
#include <iostream>
#include <thread>

namespace rtp::server
{
    void signal_handler(int signum)
    {
        if (signum == SIGINT) {
            rtp::log::info("Server received SIGINT (Ctrl+C). Initiating graceful "
                      "shutdown...");
            exit(0);
        }
    }

    uint16_t parseArguments(int argc, char **argv)
    {
        uint16_t port = 5000;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--port" && i + 1 < argc) {
                port = static_cast<uint16_t>(std::stoi(argv[++i]));
            }
        }
        return port;
    }
} // namespace rtp::server

int main(int ac, char **av)
{
    std::signal(SIGINT, rtp::server::signal_handler);

    uint16_t ServerPort = rtp::server::parseArguments(ac, av);
    try {
        rtp::log::info("Starting R-Type Server on port {}", ServerPort);

        rtp::server::ServerNetwork networkManager(ServerPort);
        rtp::server::GameManager gameManager(networkManager);

        networkManager.start();

        gameManager.gameLoop();

    } catch (const std::exception &e) {
        rtp::log::fatal("Fatal error during server startup or game loop: {}",
                   e.what());
        return 84;
    }

    rtp::log::info("Server main thread exiting.");
    return 0;
}
