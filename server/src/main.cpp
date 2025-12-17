/**
 * File   : main.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 10/12/2025
 */

#include "ServerNetwork/ServerNetwork.hpp"
#include "Game/GameManager.hpp"
#include "RType/Logger.hpp"

#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>

using namespace rtp::server;

ServerNetwork* g_networkManager = nullptr;
GameManager* g_gameManager = nullptr;

void signal_handler(int signum) {
    if (signum == SIGINT) {
        rtp::log::info("Server received SIGINT (Ctrl+C). Initiating graceful shutdown...");
        
        if (g_networkManager) {
            g_networkManager->stop();
            exit(0);
        }
    }
}

int main(void) {
    const uint16_t SERVER_PORT = 5000;
    
    std::signal(SIGINT, signal_handler);
    
    try {
        rtp::log::info("Starting R-Type Server...");
        
        ServerNetwork networkManager(SERVER_PORT); 
        GameManager gameManager(networkManager);
        
        g_networkManager = &networkManager;
        g_gameManager = &gameManager;

        networkManager.start();
         
        gameManager.gameLoop(); 
        
    } catch (const std::exception& e) {
        rtp::log::fatal("Fatal error during server startup or game loop: {}", e.what());
        return 84;
    }

    rtp::log::info("Server main thread exiting.");
    return 0;
}