/**
 * File   : PlayerMouvementSystem.cpp
 * License: MIT
 * Author : Elias
 * Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   :
 * 11/12/2025
 */

#include "Systems/PlayerMouvementSystem.hpp"

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    PlayerMouvementSystem::PlayerMouvementSystem(rtp::ecs::Registry &registry)
        : _registry(registry)
    {
    }

    void PlayerMouvementSystem::update(float dt)
    {
        (void)dt;
        auto view =
            _registry.zipView<rtp::ecs::components::Transform,
                              rtp::ecs::components::Velocity,
                              rtp::ecs::components::server::InputComponent,
                              rtp::ecs::components::EntityType,
                              rtp::ecs::components::MovementSpeed>();

        for (auto &&[tf, vel, input, type, speed] : view) {
            if (type.type != rtp::net::EntityType::Player)
                continue;

            if (speed.boostRemaining > 0.0f) {
                speed.boostRemaining -= dt;
                if (speed.boostRemaining <= 0.0f) {
                    speed.boostRemaining = 0.0f;
                    speed.multiplier = 1.0f;
                }
            }

            const float finalSpeed = speed.baseSpeed * speed.multiplier;

            float dx = 0.f, dy = 0.f;
            using Bits =
                rtp::ecs::components::server::InputComponent::InputBits;

            if (input.mask & Bits::MoveUp)
                dy -= 1.f;
            if (input.mask & Bits::MoveDown)
                dy += 1.f;
            if (input.mask & Bits::MoveLeft)
                dx -= 1.f;
            if (input.mask & Bits::MoveRight)
                dx += 1.f;

            const float len = std::sqrt(dx * dx + dy * dy);
            if (len > 0.f) {
                dx /= len;
                dy /= len;
            }

            const float targetX = dx * finalSpeed;
            const float targetY = dy * finalSpeed;

            constexpr float accel = 8.0f;
            constexpr float decel = 10.0f;

            if (dx != 0.f || dy != 0.f) {
                vel.direction.x += (targetX - vel.direction.x) * accel * dt;
                vel.direction.y += (targetY - vel.direction.y) * accel * dt;
            } else {
                vel.direction.x += (0.0f - vel.direction.x) * decel * dt;
                vel.direction.y += (0.0f - vel.direction.y) * decel * dt;
            }
        }
    }
} // namespace rtp::server
