/**
 * File   : NetworkId.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

namespace rtp::ecs
{
    template <System T, typename... Args>
    T &SystemManager::add(Args &&...args)
    {
        auto typeName = std::type_index(typeid(T));
        auto system = std::make_unique<T>(std::forward<Args>(args)...);
        this->_signatures[typeName] = T::getRequiredSignature();

        auto &ref = *system;
        this->_systems[typeName] = std::move(system);
        return ref;
    }
}
