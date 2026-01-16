/**
 * File   : NetworkId.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

namespace engine::ecs
{
    template <typename T, typename... Args>
    T &SystemManager::add(Args &&...args)
    {
        auto typeName = std::type_index(typeid(T));
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        if constexpr (requires { T::getRequiredSignature(); }) {
            this->_signatures[typeName] = T::getRequiredSignature();
        }

        auto &ref = *system;
        this->_systems[typeName] = std::move(system);
        return ref;
    }

    template <typename T, typename... Args>
    T &SystemManager::addSystem(Args &&...args)
    {
        return this->add<T>(std::forward<Args>(args)...);
    }

    template <typename T>
    T &SystemManager::getSystem()
    {
        auto typeName = std::type_index(typeid(T));
        return *static_cast<T *>(this->_systems.at(typeName).get());
    }
}
