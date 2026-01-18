/**
 * File   : EnemyAISystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/EnemyAISystem.hpp"

#include <algorithm>
#include <cmath>
#include <limits>
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

        auto transformsRes = _registry.get<ecs::components::Transform>();
        auto velocitiesRes = _registry.get<ecs::components::Velocity>();
        auto patternsRes = _registry.get<ecs::components::MouvementPattern>();
        auto typesRes = _registry.get<ecs::components::EntityType>();
        auto roomsRes = _registry.get<ecs::components::RoomId>();
        if (!transformsRes || !velocitiesRes || !patternsRes || !typesRes || !roomsRes) {
            return;
        }
        auto &transforms = transformsRes->get();
        auto &velocities = velocitiesRes->get();
        auto &patterns = patternsRes->get();
        auto &types = typesRes->get();
        auto &rooms = roomsRes->get();

        std::unordered_map<uint32_t, std::vector<std::pair<ecs::Entity, Vec2f>>> bosses;
        for (auto entity : transforms.entities()) {
            if (!types.has(entity) || !rooms.has(entity)) {
                continue;
            }
            const auto &type = types[entity];
            const auto &room = rooms[entity];
            if (type.type == net::EntityType::Boss || type.type == net::EntityType::Boss2) {
                bosses[room.id].push_back({entity, transforms[entity].position});
            }
        }

        constexpr float minAheadDefault = 250.0f;
        constexpr float minAheadBoss = 500.0f;
        constexpr float anchorDefault = 900.0f;
        constexpr float anchorBoss = 1100.0f;
        constexpr float followGain = 2.0f;

        static std::unordered_map<ecs::Entity, ecs::Entity> shieldBoss;
        static std::unordered_map<ecs::Entity, Vec2f> shieldOffsets;

        for (auto entity : transforms.entities()) {
            if (!velocities.has(entity) || !patterns.has(entity) ||
                !types.has(entity) || !rooms.has(entity)) {
                continue;
            }

            auto &tf = transforms[entity];
            auto &vel = velocities[entity];
            auto &pat = patterns[entity];
            auto &type = types[entity];
            auto &room = rooms[entity];
            if (type.type != net::EntityType::Scout &&
                type.type != net::EntityType::Tank &&
                type.type != net::EntityType::Boss &&
                type.type != net::EntityType::Boss2 &&
                type.type != net::EntityType::BossShield) {
                continue;
            }

            if (type.type == net::EntityType::BossShield) {
                auto itBosses = bosses.find(room.id);
                if (itBosses == bosses.end() || itBosses->second.empty()) {
                    continue; // no boss to follow
                }

                ecs::Entity bossEntity = ecs::NullEntity;
                Vec2f bossPos{};
                bool bossFound = false;

                auto linkIt = shieldBoss.find(entity);
                if (linkIt != shieldBoss.end()) {
                    bossEntity = linkIt->second;
                    for (const auto& [bEntity, bPos] : itBosses->second) {
                        if (bEntity == bossEntity) {
                            bossPos = bPos;
                            bossFound = true;
                            break;
                        }
                    }
                }

                if (bossEntity == ecs::NullEntity || !bossFound) {
                    float bestDist2 = std::numeric_limits<float>::max();
                    for (const auto& [bEntity, bPos] : itBosses->second) {
                        const float dx = bPos.x - tf.position.x;
                        const float dy = bPos.y - tf.position.y;
                        const float dist2 = dx * dx + dy * dy;
                        if (dist2 < bestDist2) {
                            bestDist2 = dist2;
                            bossEntity = bEntity;
                            bossPos = bPos;
                            bossFound = true;
                        }
                    }
                    shieldBoss[entity] = bossEntity;
                    shieldOffsets[entity] = tf.position - bossPos;
                }

                auto offIt = shieldOffsets.find(entity);
                if (offIt == shieldOffsets.end()) {
                    shieldOffsets[entity] = tf.position - bossPos;
                    offIt = shieldOffsets.find(entity);
                }

                const Vec2f targetPos = bossPos + offIt->second;
                const Vec2f delta = targetPos - tf.position;
                const float maxSpeed = std::max(60.0f, pat.speed);
                vel.direction.x = std::clamp(delta.x * 2.5f, -maxSpeed, maxSpeed);
                vel.direction.y = std::clamp(delta.y * 2.5f, -maxSpeed, maxSpeed);
                continue; // skip default handling
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
                const bool isBoss = (type.type == net::EntityType::Boss || type.type == net::EntityType::Boss2);
                const float minAhead = isBoss ? minAheadBoss : minAheadDefault;
                const float anchorX = isBoss ? anchorBoss : anchorDefault;

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