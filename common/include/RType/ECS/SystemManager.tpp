/**
 * File   : NetworkId.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "SystemManager.hpp"
#include "RType/Logger.hpp"
#include <type_traits>

namespace rtp::ecs
{
    template <typename T, typename... Args>
    T& SystemManager::addSystem(Args&&... args) {
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        
        auto& ref = *system;
        _systems[std::type_index(typeid(T))] = std::move(system);
        return ref;
    }
    
    template <typename T>
    auto SystemManager::getSystem(void) -> T &
    {
        auto it = _systems.find(std::type_index(typeid(T)));
        if (it == _systems.end()) {
            throw std::runtime_error("System not found");
        }

        return *static_cast<T*>(it->second.get());
    }
} // namespace rtp::ecs