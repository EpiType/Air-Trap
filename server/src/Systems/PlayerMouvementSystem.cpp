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
                              rtp::ecs::components::EntityType>();

        constexpr float speed = 200.f;

        for (auto &&[tf, vel, input, type] : view) {
            if (type.type != rtp::net::EntityType::Player)
                continue;

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

            vel.direction.x = dx * speed;
            vel.direction.y = dy * speed;
        }
    }
} // namespace rtp::server
