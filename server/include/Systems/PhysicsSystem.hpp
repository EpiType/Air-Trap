/**
 * File   : GameLoginSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josué.hajjar-llauquen@epitech.eu>
 * Date   : 10/12/2025
 */

 #ifndef PHYSICSSYSTEM_HPP
 #define PHYSICSSYSTEM_HPP

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"

namespace rtp::server::systems {
    class PhysicsSystem : public rtp::ecs::ISystem {
      public:
        /**
         * @brief Construct a new Physics System object
         */
        PhysicsSystem() = default;

        /**
         * @brief Destroy the Physics System object
         */
        ~PhysicsSystem() override = default;

        /**
         * @brief Update the physics system
         *
         * @param registry The ECS registry
         * @param deltaTime The time elapsed since the last update
         */
        void update(rtp::ecs::Registry &registry, float deltaTime) override;
    };
}  // namespace rtp::server::systems