/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SystemManager.hpp
*/

#ifndef RTYPE_SYSTEMMANAGER_HPP_
    #define RTYPE_SYSTEMMANAGER_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "RType/ECS/Signature.hpp"
    #include <memory>
    #include <unordered_map>
    #include <typeindex>


namespace rtp::ecs
{
    class SystemManager {
    public:
        template <typename T>
        void registerSystem(void);

        template <typename T>
        auto getSystem(void) -> T &;

        template <typename T>
        void setSignature(Signature signature);

        void update(Registry &registry, float dt);

    private:
        Registry *_registry{nullptr}; 

        std::unordered_map<std::type_index,
                           std::unique_ptr<ISystem>> _systems;
        std::unordered_map<std::type_index, Signature> _signatures;
    };
}

#endif /* !RTYPE_SYSTEMMANAGER_HPP_ */
