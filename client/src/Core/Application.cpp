/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
** Application.cpp, main application loop
*/

#include "Core/Application.hpp"
#include "RType/Logger.hpp"

namespace Client::Core
{
    Application::Application()
        : _window(sf::VideoMode({1280, 720}), "Air-Trap - R-Type Clone")
    {
        initECS();
        this->_window.setFramerateLimit(60);
        rtp::log::info("Window created: 1280x720 @ 60 FPS");
    }

    void Application::run()
    {
        sf::Clock clock;
        rtp::log::info("Starting game loop");

        while (this->_window.isOpen()) {
            sf::Time deltaTime = clock.restart();

            this->processInput();
            this->update(deltaTime);
            this->render();
        }

        rtp::log::info("Game loop ended");
    }

    void Application::initECS()
    {
        // TODO: Initialize ECS systems when ready
        rtp::log::info("ECS initialization (placeholder)");
    }

    void Application::processInput()
    {
        while (auto event = this->_window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                rtp::log::info("Window close requested");
                this->_window.close();
            }

            // ESC key to quit
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape) {
                    rtp::log::info("Escape key pressed, closing window");
                    this->_window.close();
                }
            }
        }
    }

    void Application::update(sf::Time delta)
    {
        (void)delta; // Unused for now
        // TODO: Update game logic
        // For now, just empty game loop
    }

    void Application::render()
    {
        // Clear screen with dark blue (space-like background)
        this->_window.clear(sf::Color(10, 10, 30));
        if (_renderSystem)
            _renderSystem->render(this->_window, sf::seconds(1.f / 60.f));

        // TODO: Render entities when ECS is ready

        this->_window.display();
    }
}
