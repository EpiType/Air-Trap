/**
 * File   : EnemyAISystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/EnemyAISystem.hpp"

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    EnemyAISystem::EnemyAISystem(rtp::ecs::Registry& registry)
        : _registry(registry) {}

    void EnemyAISystem::update(float dt)
    {
        _time += dt;
        
        auto view = _registry.zipView<
            rtp::ecs::components::Velocity,
            rtp::ecs::components::MouvementPattern,
            rtp::ecs::components::EntityType
        >();

        for (auto&& [vel, pat, type] : view) {
            if (type.type != rtp::net::EntityType::Scout)
                continue;

            switch (pat.pattern) {
                case rtp::ecs::components::Patterns::StraightLine:
                    vel.velocity.x = -pat.speed;
                    vel.velocity.y = 0.f;
                    break;

                case rtp::ecs::components::Patterns::SineWave:
                    vel.velocity.x = -pat.speed;
                    vel.velocity.y = pat.amplitude * std::sin(_time * pat.frequency);
                    break;

                case rtp::ecs::components::Patterns::Static:
                    vel.velocity.x = 0.f;
                    vel.velocity.y = 0.f;
                    break;

                default:
                    // after
                    break;
            }
        }
    }
} // namespace rtp::server