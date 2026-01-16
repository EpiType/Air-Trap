/**
 * File   : SystemManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "engine/ecs/SystemManager.hpp"
#include "engine/core/Logger.hpp"

namespace engine::ecs
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
        for (auto &[type, system] : this->_systems) {
            const auto signatureIt = this->_signatures.find(type);
            if (signatureIt != this->_signatures.end() &&
                !this->_registry.anyMatch(signatureIt->second)) {
                continue;
            }
            system->update(dt);
        }
    }

}
