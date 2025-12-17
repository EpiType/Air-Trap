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
            rtp::ecs::components::server::InputComponent
        >();

        constexpr float speed = 200.f;

        for (auto&& [tf, input] : view) {
            float dx = 0.f;
            float dy = 0.f;

            using Bits = rtp::ecs::components::server::InputComponent::InputBits;

            if (input.mask & Bits::MoveUp)    dy -= 1.f; /* Up */
            if (input.mask & Bits::MoveDown)  dy += 1.f; /* Down */
            if (input.mask & Bits::MoveLeft)  dx -= 1.f; /* Left */
            if (input.mask & Bits::MoveRight) dx += 1.f; /* Right */

            tf.position.x += dx * speed * dt;
            tf.position.y += dy * speed * dt;
        }
    }
} // namespace rtp::server