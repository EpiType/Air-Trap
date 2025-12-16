/**
 * File   : SystemManager.tpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "SystemManager.hpp"
#include "RType/Logger.hpp"
#include <type_traits>

namespace rtp::ecs
{
    template <typename T, typename... Args>
    T& SystemManager::addSystem(Args&&... args) {
        std::unique_ptr<T> system;
        
        if constexpr (std::is_constructible_v<T, Registry&, Args...>) {
            system = std::make_unique<T>(_registry, std::forward<Args>(args)...);
        }
        else {
            system = std::make_unique<T>(_registry);
        }
        
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

