/*
** EPITECH PROJECT, 2025
** R-Type, Client
** File description:
** main.cpp, entry point for the client application
*/

#include <iostream>

#include "Core/Application.hpp"
#include "RType/Logger.hpp"

int main()
{
    // Configure logger
    if (auto result = rtp::log::configure("logs/client.log");
        !result.has_value()) {
        std::cerr << "Failed to configure logger\n";
        return 1;
    }

    rtp::log::info("R-Type Client starting...");

    try {
        // Create and run the application
        Client::Core::Application app;
        app.run();
    } catch (const std::exception& e) {
        rtp::log::error("Fatal error: {}", e.what());
        return 1;
    }

    rtp::log::info("Client shut down successfully");
    return 0;
}
