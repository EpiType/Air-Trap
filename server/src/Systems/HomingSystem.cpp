/**
 * File   : HomingSystem.cpp
 * License: MIT
 */

#include "Systems/HomingSystem.hpp"
#include "RType/Math/Vec2.hpp"
#include "RType/Network/Packet.hpp"
#include <limits>

namespace rtp::server
{
    HomingSystem::HomingSystem(ecs::Registry &registry)
        : _registry(registry)
    {}

    void HomingSystem::update(float dt)
    {
        using ecs::components::Transform;
        using ecs::components::Velocity;
        using ecs::components::EntityType;
        using ecs::components::RoomId;
        using ecs::components::Health;
        using ecs::components::Homing;

        // Early out if registry missing required arrays
        auto homingRes = _registry.get<Homing>();
        auto tfRes = _registry.get<Transform>();
        auto velRes = _registry.get<Velocity>();
        auto roomRes = _registry.get<RoomId>();
        if (!homingRes || !tfRes || !velRes || !roomRes)
            return;

        auto &homings = homingRes->get();
        auto &transforms = tfRes->get();
        auto &vels = velRes->get();
        auto &rooms = roomRes->get();

        // We'll iterate candidates for targets
        auto targetView = _registry.zipView<Transform, EntityType, Health, RoomId>();

        for (auto &&[homing, tf, vel, room] : _registry.zipView<Homing, Transform, Velocity, RoomId>()) {
            // find nearest hostile (Scout/Tank/Boss) in same room
            float bestDist2 = std::numeric_limits<float>::infinity();
            bool found = false;
            rtp::Vec2f targetPos{0.0f, 0.0f};

            for (auto &&[otf, otype, ohealth, oroom] : targetView) {
                if (oroom.id != room.id)
                    continue;
                // skip players and non-enemies
                if (otype.type == net::EntityType::Player)
                    continue;
                if (otype.type != net::EntityType::Scout && otype.type != net::EntityType::Tank && otype.type != net::EntityType::Boss)
                    continue;

                const float dx = otf.position.x - tf.position.x;
                const float dy = otf.position.y - tf.position.y;
                const float d2 = dx * dx + dy * dy;
                if (d2 < bestDist2 && d2 <= homing.range * homing.range) {
                    bestDist2 = d2;
                    targetPos.x = otf.position.x;
                    targetPos.y = otf.position.y;
                    found = true;
                }
            }

            if (!found)
                continue;

            // Compute desired normalized direction
            rtp::Vec2f desired = rtp::Vec2f{targetPos.x - tf.position.x, targetPos.y - tf.position.y}.normalized();

            if (vel.speed > 0.0f) {
                // direction is normalized
                rtp::Vec2f curr = vel.direction;
                if (curr.squaredLength() == 0.0f) curr = desired;
                else curr = curr.normalized();

                // simple lerp by steering*dt
                const float t = std::min(1.0f, homing.steering * dt);
                rtp::Vec2f newDir = (curr * (1.0f - t) + desired * t).normalized();
                vel.direction = newDir;
            } else {
                // direction is velocity vector in pixels/sec
                const float currSpeed = vel.direction.length();
                rtp::Vec2f currDir = currSpeed == 0.0f ? desired : vel.direction / currSpeed;
                const float t = std::min(1.0f, homing.steering * dt);
                rtp::Vec2f newDir = (currDir * (1.0f - t) + desired * t).normalized();
                vel.direction = newDir * currSpeed;
            }
        }
    }

} // namespace rtp::server
