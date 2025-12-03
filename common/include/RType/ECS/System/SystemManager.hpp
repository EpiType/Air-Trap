/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** SystemManager
*/

#pragma once

#include <cassert>
#include <memory>
#include <unordered_map>

#include "RType/ECS/Component.hpp"
#include "RType/ECS/System/System.hpp"
#include "RType/ECS/Types.hpp"

namespace rtp::ecs {
class SystemManager {
   public:
    template <typename T>
    std::shared_ptr<T> registerSystem() {
        const char* typeName = typeid(T).name();

        assert(_Systems.find(typeName) == _Systems.end() && "Registering system more than once.");

        auto system = std::make_shared<T>();
        _Systems.insert({typeName, system});
        return system;
    }

    template <typename T>
    void setSignature(Signature signature) {
        const char* typeName = typeid(T).name();

        assert(_Systems.find(typeName) != _Systems.end() && "System used before registered.");

        _Signatures.insert({typeName, signature});
    }

    void entityDestroyed(Entity entity) {
        for (auto const& pair : _Systems) {
            auto const& system = pair.second;
            system->_Entities.erase(entity);
        }
    }

    void entitySignatureChanged(Entity entity, Signature entitySignature) {
        for (auto const& pair : _Systems) {
            auto const& type = pair.first;
            auto const& system = pair.second;
            auto const& systemSignature = _Signatures[type];

            if ((entitySignature & systemSignature) == systemSignature) {
                system->_Entities.insert(entity);
            } else {
                system->_Entities.erase(entity);
            }
        }
    }

   private:
    std::unordered_map<const char*, Signature> _Signatures{};            /**< Map of system type to their signatures */
    std::unordered_map<const char*, std::shared_ptr<System>> _Systems{}; /**< Map of system type to their instances */
};
}  // namespace rtp::ecs