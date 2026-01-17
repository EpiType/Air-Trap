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

    EnemyAISystem::EnemyAISystem(ecs::Registry& registry)
        : _registry(registry) {}

    void EnemyAISystem::update(float dt)
    {
        _time += dt;

        std::unordered_map<uint32_t, float> frontX;
        {
            auto players = _registry.zipView<
                ecs::components::Transform,
                ecs::components::EntityType,
                ecs::components::RoomId
            >();
            for (auto&& [tf, type, room] : players) {
                if (type.type != net::EntityType::Player) {
                    continue;
                }
                auto it = frontX.find(room.id);
                if (it == frontX.end() || tf.position.x > it->second) {
                    frontX[room.id] = tf.position.x;
                }
            }
        }

        auto view = _registry.zipView<
            ecs::components::Transform,
            ecs::components::Velocity,
            ecs::components::MouvementPattern,
            ecs::components::EntityType,
            ecs::components::RoomId
        >();

        constexpr float minAhead = 250.0f;
        constexpr float anchorX = 900.0f;
        constexpr float followGain = 2.0f;

        for (auto&& [tf, vel, pat, type, room] : view) {
            if (type.type != net::EntityType::Scout &&
                type.type != net::EntityType::Tank &&
                type.type != net::EntityType::Boss) {
                continue;
            }
            if (pat.pattern != ecs::components::Patterns::Kamikaze &&
                tf.position.x < frontX[room.id] - 100.0f) {
                continue;
            }
            if (pat.pattern == ecs::components::Patterns::Kamikaze) {
                auto it = frontX.find(room.id);
                if (it != frontX.end()) {
                    const float dx = it->second - tf.position.x;
                    vel.direction.x = std::clamp(dx * followGain, -pat.speed, pat.speed);
                } else {
                    vel.direction.x = 0.f;
                }
            } else {
                float targetX = anchorX;
                auto it = frontX.find(room.id);
                if (it != frontX.end()) {
                    targetX = std::max(anchorX, it->second + minAhead);
                }
                const float dx = targetX - tf.position.x;
                const float maxXSpeed = std::max(60.0f, pat.speed);
                const float desiredX = std::clamp(dx * followGain, -maxXSpeed, maxXSpeed);
                vel.direction.x = desiredX;
            }

            switch (pat.pattern) {
                case ecs::components::Patterns::StraightLine:
                    vel.direction.y = 0.f;
                    break;

                case ecs::components::Patterns::SineWave:
                    vel.direction.y = pat.amplitude * std::sin(_time * pat.frequency);
                    break;

                case ecs::components::Patterns::ZigZag: {
                    const float zigzagPeriod = 2.0f;
                    const float phase = std::fmod(_time * pat.frequency, zigzagPeriod);
                    vel.direction.y = (phase < zigzagPeriod / 2.0f) ? pat.amplitude : -pat.amplitude;
                    break;
                }

                case ecs::components::Patterns::Circular: {
                    const float angle = _time * pat.frequency;
                    const float circleX = pat.amplitude * std::cos(angle);
                    const float circleY = pat.amplitude * std::sin(angle);
                    vel.direction.x += circleX * 4.0f;
                    vel.direction.y = circleY * 2.0f;
                    break;
                }

                case ecs::components::Patterns::Kamikaze: {
                    float nearestDist = std::numeric_limits<float>::max();
                    Vec2f nearestPlayerPos = {tf.position.x, tf.position.y};
                    
                    auto players = _registry.zipView<
                        ecs::components::Transform,
                        ecs::components::EntityType,
                        ecs::components::RoomId
                    >();
                    
                    for (auto&& [pTf, pType, pRoom] : players) {
                        if (pType.type == net::EntityType::Player && pRoom.id == room.id) {
                            const float dx = pTf.position.x - tf.position.x;
                            const float dy = pTf.position.y - tf.position.y;
                            const float dist = std::sqrt(dx * dx + dy * dy);
                            if (dist < nearestDist) {
                                nearestDist = dist;
                                nearestPlayerPos = pTf.position;
                            }
                        }
                    }
                    const float dx = nearestPlayerPos.x - tf.position.x;
                    const float dy = nearestPlayerPos.y - tf.position.y;
                    const float distance = std::sqrt(dx * dx + dy * dy);
                    
                    if (distance > 0.1f) {
                        vel.direction.x = (dx / distance) * pat.speed;
                        vel.direction.y = (dy / distance) * pat.speed;
                    } else {
                        vel.direction.x = 0.f;
                        vel.direction.y = 0.f;
                    }
                    break;
                }

                case ecs::components::Patterns::Static:
                    vel.direction.y = 0.f;
                    break;

                default:
                    break;
            }
        }
    }
} // namespace rtp::server
