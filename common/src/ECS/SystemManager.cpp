/**
 * File   : SystemManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "RType/ECS/SystemManager.hpp"
#include "RType/Logger.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

namespace rtp::ecs {

    //////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////

    SystemManager::SystemManager(Registry &registry) 
        : _registry(registry)
    {}

    void SystemManager::setWindow(sf::RenderWindow& window) {
        _window = std::ref(window);
    }

    void SystemManager::update(float dt) {
        for (auto &[type, system] : _systems) {
            system->update(dt);
        }
    }
}