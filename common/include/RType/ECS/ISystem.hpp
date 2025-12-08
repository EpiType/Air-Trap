/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ISystem.hpp
*/

#ifndef RTYPE_ISYSTEM_HPP_
    #define RTYPE_ISYSTEM_HPP_

    #include "RType/ECS/Registry.hpp"

namespace rtp::ecs
{
    class ISystem {
        public:
            virtual ~ISystem() noexcept = default;

            virtual void update(Registry &registry, float deltaTime) = 0;
    };
}

#endif /* !RTYPE_ISYSTEM_HPP_ */
