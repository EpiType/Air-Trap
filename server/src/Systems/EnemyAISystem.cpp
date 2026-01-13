/**
 * File   : EnemyAISystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/EnemyAISystem.hpp"

#include <algorithm>
#include <cmath>
#include <unordered_map>

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

        std::unordered_map<uint32_t, float> frontX;
        {
            auto players = _registry.zipView<
                rtp::ecs::components::Transform,
                rtp::ecs::components::EntityType,
                rtp::ecs::components::RoomId
            >();
            for (auto&& [tf, type, room] : players) {
                if (type.type != rtp::net::EntityType::Player) {
                    continue;
                }
                auto it = frontX.find(room.id);
                if (it == frontX.end() || tf.position.x > it->second) {
                    frontX[room.id] = tf.position.x;
                }
            }
        }

        auto view = _registry.zipView<
            rtp::ecs::components::Transform,
            rtp::ecs::components::Velocity,
            rtp::ecs::components::MouvementPattern,
            rtp::ecs::components::EntityType,
            rtp::ecs::components::RoomId
        >();

        constexpr float minAhead = 250.0f;
        constexpr float anchorX = 900.0f;
        constexpr float followGain = 2.0f;

        for (auto&& [tf, vel, pat, type, room] : view) {
            if (type.type != rtp::net::EntityType::Scout &&
                type.type != rtp::net::EntityType::Tank &&
                type.type != rtp::net::EntityType::Boss) {
                continue;
            }

            float targetX = anchorX;
            auto it = frontX.find(room.id);
            if (it != frontX.end()) {
                targetX = std::max(anchorX, it->second + minAhead);
            }

            const float dx = targetX - tf.position.x;
            const float maxXSpeed = std::max(60.0f, pat.speed);
            const float desiredX = std::clamp(dx * followGain, -maxXSpeed, maxXSpeed);

            vel.direction.x = desiredX;

            switch (pat.pattern) {
                case rtp::ecs::components::Patterns::StraightLine:
                    vel.direction.y = 0.f;
                    break;

                case rtp::ecs::components::Patterns::SineWave:
                    vel.direction.y = pat.amplitude * std::sin(_time * pat.frequency);
                    break;

                case rtp::ecs::components::Patterns::Static:
                    vel.direction.y = 0.f;
                    break;

                default:
                    break;
            }
        }
    }
} // namespace rtp::server
