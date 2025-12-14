/**
 * File   : MovementSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_CLIENT_MOVEMENTSYSTEM_HPP_
    #define RTYPE_CLIENT_MOVEMENTSYSTEM_HPP_

    #include "RType/ECS/Registry.hpp"
    #include "RType/ECS/ISystem.hpp"

    // Inclusion stricte de tes composants
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"

namespace rtp::client {

    /**
     * @class MovementSystem
     * @brief System responsible for updating entity positions based on their velocity.
     */
    class MovementSystem : public rtp::ecs::ISystem {
        public:
            /**
             * @brief Construct a new Movement System
             * @param registry Reference to the ECS Registry
             */
            explicit MovementSystem(rtp::ecs::Registry& registry) 
                : _registry(registry) {}

            /**
             * @brief Update positions based on velocity and delta time
             * @param dt Delta time in seconds (Time elapsed since last frame)
             */
            void update(float dt) override {
                auto view = _registry.zipView<rtp::ecs::components::Transform, rtp::ecs::components::Velocity>();

                for (auto&& [transform, velocity] : view) {
                    transform.position.x += velocity.direction.x * velocity.speed * dt;
                    transform.position.y += velocity.direction.y * velocity.speed * dt;
                }
            }

        private:
            rtp::ecs::Registry& _registry;
    };

} // namespace rtp::client

#endif /* !RTYPE_CLIENT_MOVEMENTSYSTEM_HPP_ */