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

        auto playerView = _registry.zipView<
            rtp::ecs::components::Transform,
            rtp::ecs::components::BoundingBox,
            rtp::ecs::components::EntityType
        >();

        constexpr float kWindowWidth = 1280.0f;
        constexpr float kWindowHeight = 720.0f;

        for (auto&& [tf, box, type] : playerView) {
            if (type.type != rtp::net::EntityType::Player) {
                continue;
            }

            if (tf.position.x < 0.0f) tf.position.x = 0.0f;
            if (tf.position.y < 0.0f) tf.position.y = 0.0f;

            const float maxX = kWindowWidth - box.width;
            const float maxY = kWindowHeight - box.height;

            if (tf.position.x > maxX) tf.position.x = maxX;
            if (tf.position.y > maxY) tf.position.y = maxY;
        }
    }
} // namespace rtp::server
