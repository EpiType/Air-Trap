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
        
        // Detect if T can be constructed with (Registry&, sf::RenderWindow&, Args...)
        if constexpr (std::is_constructible_v<T, Registry&, sf::RenderWindow&, Args...>) {
            // System needs window + custom args
            if (!_window.has_value()) {
                rtp::log::error("SystemManager: Window not set but required by {}", typeid(T).name());
                throw std::runtime_error("Window context not set in SystemManager");
            }
            system = std::make_unique<T>(_registry, _window->get(), std::forward<Args>(args)...);
        }
        // Detect if T can be constructed with (Registry&, sf::RenderWindow&)
        else if constexpr (std::is_constructible_v<T, Registry&, sf::RenderWindow&>) {
            // System needs window
            if (!_window.has_value()) {
                rtp::log::error("SystemManager: Window not set but required by {}", typeid(T).name());
                throw std::runtime_error("Window context not set in SystemManager");
            }
            system = std::make_unique<T>(_registry, _window->get());
        } 
        else if constexpr (std::is_constructible_v<T, Registry&, Args...>) {
            // System with custom args
            system = std::make_unique<T>(_registry, std::forward<Args>(args)...);
        }
        else {
            // Simple system (only Registry)
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

