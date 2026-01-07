/**
 * File   : GameManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/MovementSystem.hpp"

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    MovementSystem::MovementSystem(rtp::ecs::Registry& registry)
        : _registry(registry) {}

    void MovementSystem::update(float dt)
    {
        auto view = _registry.zipView<
            rtp::ecs::components::Transform,
            rtp::ecs::components::Velocity
        >();

        for (auto&& [tf, vel] : view) {
            tf.position.x += vel.direction.x * dt;
            tf.position.y += vel.direction.y * dt;
        }
    }
} // namespace rtp::server