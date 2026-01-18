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

    MovementSystem::MovementSystem(ecs::Registry& registry)
        : _registry(registry) {}

    void MovementSystem::update(float dt)
    {
        auto view = _registry.zipView<
            ecs::components::Transform,
            ecs::components::Velocity
        >();

        for (auto&& [tf, vel] : view) {
            // Handle both conventions:
            // 1. Old: direction contains velocity (pixels/sec), speed=0
            // 2. New: direction is normalized, speed is separate
            if (vel.speed > 0.0f) {
                tf.position.x += vel.direction.x * vel.speed * dt;
                tf.position.y += vel.direction.y * vel.speed * dt;
            } else {
                tf.position.x += vel.direction.x * dt;
                tf.position.y += vel.direction.y * dt;
            }
        }

        auto playerView = _registry.zipView<
            ecs::components::Transform,
            ecs::components::BoundingBox,
            ecs::components::EntityType
        >();

        constexpr float kWindowWidth = 1280.0f;
        constexpr float kWindowHeight = 720.0f;

        for (auto&& [tf, box, type] : playerView) {
            if (type.type != net::EntityType::Player) {
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
