/**
 * File   : BoomerangSystem.cpp
 * License: MIT
 */

#include "Systems/BoomerangSystem.hpp"
#include "RType/Logger.hpp"

namespace rtp::server
{
    BoomerangSystem::BoomerangSystem(ecs::Registry &registry)
        : _registry(registry)
    {}

    void BoomerangSystem::update(float dt)
    {
        using ecs::components::Boomerang;
        using ecs::components::Transform;
        using ecs::components::Velocity;

        auto boomerRes = _registry.get<Boomerang>();
        auto tfRes = _registry.get<Transform>();
        auto velRes = _registry.get<Velocity>();

        if (!boomerRes || !tfRes || !velRes)
            return;

        auto &boomers = boomerRes->get();
        auto &transforms = tfRes->get();
        auto &vels = velRes->get();

        for (auto e : boomers.entities()) {
            if (!boomers.has(e) || !transforms.has(e) || !vels.has(e))
                continue;

            auto &b = boomers[e];
            auto &btf = transforms[e];
            auto &bvel = vels[e];

            // If not yet returning, check distance from start
            if (!b.returning) {
                const float dx = btf.position.x - b.startPos.x;
                const float dy = btf.position.y - b.startPos.y;
                const float d2 = dx * dx + dy * dy;
                if (d2 >= b.maxDistance * b.maxDistance) {
                    b.returning = true;
                }
            }

            // If returning, steer towards owner
            if (b.returning) {
                ecs::Entity owner{b.ownerIndex, 0};
                if (!transforms.has(owner)) {
                    // Owner no longer exists: remove projectile
                    _registry.kill(ecs::Entity{static_cast<uint32_t>(e.index()), 0});
                    continue;
                }

                const auto &otf = transforms[owner];
                rtp::Vec2f desired{otf.position.x - btf.position.x, otf.position.y - btf.position.y};
                if (bvel.speed > 0.0f) {
                    desired.normalize();
                    bvel.direction = desired; // maintain speed scalar
                } else {
                    const float sp = bvel.direction.length();
                    if (sp == 0.0f) {
                        // fallback to a normalized vector
                        desired = desired.normalized();
                        bvel.direction = desired;
                    } else {
                        desired = desired.normalized();
                        bvel.direction = desired * sp;
                    }
                }
            }
        }
    }

} // namespace rtp::server
