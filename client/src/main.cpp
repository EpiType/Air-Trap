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
        std::println(std::cerr, "\033[31m[LOGGER ERROR] Failed to "
                             "configure logger: {}\033[0m",
                     result.error().message());
        return 84;
    }

    rtp::log::info("R-Type Client starting...");

    try {
        // Create and run the application
        Client::Core::Application app;
        app.run();
    } catch (const std::exception& e) {
        rtp::log::error("Fatal error: {}", e.what());
        return 84;
    }

    rtp::log::info("Client shut down successfully");
    return 0;
}
