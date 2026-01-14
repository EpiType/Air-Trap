/**
 * File   : SystemManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "RType/ECS/SystemManager.hpp"
#include "RType/Logger.hpp"

namespace rtp::ecs
{
    //////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////

    SystemManager::SystemManager(Registry &registry) 
        : _registry(registry)
    {
    }

    void SystemManager::update(float dt)
    {
        for (auto &[type, system] : this->_systems)
            system->update(dt);
    }

}
