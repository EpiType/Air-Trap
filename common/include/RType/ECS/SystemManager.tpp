/**
 * File   : SystemManager.tpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "SystemManager.hpp"
#include "RType/Logger.hpp"

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
        return *static_cast<T*>(_systems[std::type_index(typeid(T))].get());
    }
} // namespace rtp::ecs

