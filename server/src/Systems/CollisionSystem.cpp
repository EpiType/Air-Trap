/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
**
 * CollisionSystem
*/

#include "Systems/CollisionSystem.hpp"

#include "RType/Logger.hpp"

#include <algorithm>
#include <cmath>
#include <vector>

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////
    CollisionSystem::CollisionSystem(ecs::Registry &registry,
                                     RoomSystem &roomSystem,
                                     NetworkSyncSystem &networkSync)
        : _registry(registry)
        , _roomSystem(roomSystem)
        , _networkSync(networkSync)
    {
    }

    void CollisionSystem::update(float dt)
    {
        (void)dt;
        auto transformsRes = _registry.get<ecs::components::Transform>();
        auto boxesRes = _registry.get<ecs::components::BoundingBox>();
        auto typesRes = _registry.get<ecs::components::EntityType>();
        auto roomsRes = _registry.get<ecs::components::RoomId>();
        auto healthRes = _registry.get<ecs::components::Health>();
        auto speedRes = _registry.get<ecs::components::MovementSpeed>();
        auto powerupRes = _registry.get<ecs::components::Powerup>();
        auto damageRes = _registry.get<ecs::components::Damage>();
        auto velocityRes = _registry.get<ecs::components::Velocity>();

        if (!transformsRes ||
            !boxesRes ||
            !typesRes ||
            !roomsRes ||
            !healthRes ||
            !speedRes ||
            !powerupRes ||
            !damageRes ||
            !velocityRes) {
            return;
        }

        auto &transforms = transformsRes->get();
        auto &boxes = boxesRes->get();
        auto &types = typesRes->get();
        auto &rooms = roomsRes->get();
        auto &healths = healthRes->get();
        auto &speeds = speedRes->get();
        auto &powerups = powerupRes->get();
        auto &damages = damageRes->get();
        auto &velocities = velocityRes->get();

        std::unordered_set<uint32_t> removed;
        std::vector<std::pair<ecs::Entity, uint32_t>> pending;
        std::vector<ecs::Entity> players;
        std::vector<ecs::Entity> enemies;
        std::vector<ecs::Entity> obstacles;
        std::vector<ecs::Entity> bullets;
        std::vector<ecs::Entity> enemyBullets;
        std::vector<ecs::Entity> powerupEntities;

        auto markForDespawn = [&](ecs::Entity entity, uint32_t roomId) {
            if (removed.find(entity.index()) != removed.end()) {
                return;
            }
            removed.insert(entity.index());
            pending.emplace_back(entity, roomId);
        };

        for (auto entity : types.entities()) {
            if (!types.has(entity)) {
                continue;
            }
            const auto type = types[entity].type;
            if (type == net::EntityType::Player) {
                if (transforms.has(entity) &&
                    boxes.has(entity) &&
                    rooms.has(entity) &&
                    healths.has(entity) &&
                    speeds.has(entity)) {
                    players.push_back(entity);
                }
            } else if (type ==
                       net::EntityType::Scout ||
                       type ==
                       net::EntityType::Tank ||
                       type == net::EntityType::Boss) {
                if (transforms.has(entity) &&
                    boxes.has(entity) &&
                    rooms.has(entity) &&
                    healths.has(entity)) {
                    enemies.push_back(entity);
                }
            } else if (type ==
                       net::EntityType::Obstacle ||
                       type == net::EntityType::ObstacleSolid) {
                if (transforms.has(entity) &&
                    boxes.has(entity) &&
                    rooms.has(entity) &&
                    healths.has(entity)) {
                    obstacles.push_back(entity);
                }
            } else if (type ==
                       net::EntityType::Bullet ||
                       type == net::EntityType::ChargedBullet) {
                if (transforms.has(entity) &&
                    boxes.has(entity) &&
                    rooms.has(entity) &&
                    damages.has(entity)) {
                    bullets.push_back(entity);
                }
            } else if (type == net::EntityType::EnemyBullet) {
                if (transforms.has(entity) &&
                    boxes.has(entity) &&
                    rooms.has(entity) &&
                    damages.has(entity)) {
                    enemyBullets.push_back(entity);
                }
            } else if (type ==
                       net::EntityType::PowerupHeal ||
                       type == net::EntityType::PowerupSpeed) {
                if (transforms.has(entity) &&
                    boxes.has(entity) &&
                    rooms.has(entity) &&
                    powerups.has(entity)) {
                    powerupEntities.push_back(entity);
                }
            }
        }

        for (auto player : players) {
            auto &ptf = transforms[player];
            auto &pbox = boxes[player];
            auto &proom = rooms[player];
            auto &health = healths[player];
            auto &speed = speeds[player];
            auto *pvel = velocities.has(player) ? &velocities[player] : nullptr;

            for (auto powerEntity : powerupEntities) {
                if (removed.find(powerEntity.index()) != removed.end()) {
                    continue;
                }
                if (rooms[powerEntity].id != proom.id) {
                    continue;
                }

                const auto &tf = transforms[powerEntity];
                const auto &box = boxes[powerEntity];
                if (!overlaps(ptf, pbox, tf, box)) {
                    continue;
                }

                const auto &powerup = powerups[powerEntity];
                if (powerup.type == ecs::components::PowerupType::Heal) {
                    health.currentHealth = std::min(
                        health.maxHealth,
                        health.currentHealth + static_cast<int>(powerup.value));
                } else if (powerup.type ==
                           ecs::components::PowerupType::Speed) {
                    speed.multiplier =
                        std::max(speed.multiplier, powerup.value);
                    speed.boostRemaining =
                        std::max(speed.boostRemaining, powerup.duration);
                }

                markForDespawn(powerEntity, proom.id);
            }
        }

        for (auto player : players) {
            auto &ptf = transforms[player];
            auto &pbox = boxes[player];
            auto &proom = rooms[player];
            auto *pvel = velocities.has(player) ? &velocities[player] : nullptr;

            for (auto obstacle : obstacles) {
                if (rooms[obstacle].id != proom.id) {
                    continue;
                }
                const auto &otf = transforms[obstacle];
                const auto &obox = boxes[obstacle];
                if (!overlaps(ptf, pbox, otf, obox)) {
                    continue;
                }

                const float px = ptf.position.x;
                const float py = ptf.position.y;
                const float pw = pbox.width;
                const float ph = pbox.height;

                const float ox = otf.position.x;
                const float oy = otf.position.y;
                const float ow = obox.width;
                const float oh = obox.height;

                const float overlapX1 = (px + pw) - ox;
                const float overlapX2 = (ox + ow) - px;
                const float overlapY1 = (py + ph) - oy;
                const float overlapY2 = (oy + oh) - py;

                const float minOverlapX =
                    (overlapX1 < overlapX2) ? overlapX1 : -overlapX2;
                const float minOverlapY =
                    (overlapY1 < overlapY2) ? overlapY1 : -overlapY2;

                if (std::abs(minOverlapX) < std::abs(minOverlapY)) {
                    ptf.position.x -= minOverlapX;
                    if (pvel) {
                        pvel->direction.x = 0.0f;
                    }
                } else {
                    ptf.position.y -= minOverlapY;
                    if (pvel) {
                        pvel->direction.y = 0.0f;
                    }
                }
            }
        }

        for (auto bullet : bullets) {
            if (removed.find(bullet.index()) != removed.end()) {
                continue;
            }
            const auto &btf = transforms[bullet];
            const auto &bbox = boxes[bullet];
            const auto &broom = rooms[bullet];
            const auto &damage = damages[bullet];

            for (auto enemy : enemies) {
                if (rooms[enemy].id != broom.id) {
                    continue;
                }
                const auto &etf = transforms[enemy];
                const auto &ebox = boxes[enemy];
                auto &health = healths[enemy];
                if (!overlaps(btf, bbox, etf, ebox)) {
                    continue;
                }

                health.currentHealth -= damage.amount;
                markForDespawn(bullet, broom.id);
                if (health.currentHealth <= 0) {
                    markForDespawn(enemy, broom.id);
                }
                break;
            }

            if (removed.find(bullet.index()) != removed.end()) {
                continue;
            }

            for (auto obstacle : obstacles) {
                if (rooms[obstacle].id != broom.id) {
                    continue;
                }
                const auto &otf = transforms[obstacle];
                const auto &obox = boxes[obstacle];
                auto &health = healths[obstacle];
                if (!overlaps(btf, bbox, otf, obox)) {
                    continue;
                }

                if (types[obstacle].type == net::EntityType::Obstacle) {
                    health.currentHealth -= damage.amount;
                }
                markForDespawn(bullet, broom.id);
                if (types[obstacle].type ==
                    net::EntityType::Obstacle &&
                    health.currentHealth <= 0) {
                    markForDespawn(obstacle, broom.id);
                }
                break;
            }
        }

        for (auto bullet : enemyBullets) {
            if (removed.find(bullet.index()) != removed.end()) {
                continue;
            }
            const auto &btf = transforms[bullet];
            const auto &bbox = boxes[bullet];
            const auto &broom = rooms[bullet];
            const auto &damage = damages[bullet];

            for (auto player : players) {
                if (rooms[player].id != broom.id) {
                    continue;
                }
                const auto &ptf = transforms[player];
                const auto &pbox = boxes[player];
                auto &health = healths[player];
                if (!overlaps(btf, bbox, ptf, pbox)) {
                    continue;
                }

                health.currentHealth =
                    std::max(0, health.currentHealth - damage.amount);
                markForDespawn(bullet, broom.id);
                break;
            }
        }

        for (const auto &[entity, roomId] : pending) {
            despawn(entity, roomId);
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Private API
    /////////////////////////////////////////////////////////////////////////////

    bool CollisionSystem::overlaps(const ecs::components::Transform &a,
                                   const ecs::components::BoundingBox &ab,
                                   const ecs::components::Transform &b,
                                   const ecs::components::BoundingBox &bb) const
    {
        const float ax1 = a.position.x;
        const float ay1 = a.position.y;
        const float ax2 = a.position.x + ab.width;
        const float ay2 = a.position.y + ab.height;

        const float bx1 = b.position.x;
        const float by1 = b.position.y;
        const float bx2 = b.position.x + bb.width;
        const float by2 = b.position.y + bb.height;

        return ax1 < bx2 && ax2 > bx1 && ay1 < by2 && ay2 > by1;
    }

    void CollisionSystem::despawn(const ecs::Entity &entity, uint32_t roomId)
    {
        auto transformRes = _registry.get<ecs::components::Transform>();
        auto typeRes = _registry.get<ecs::components::EntityType>();
        auto netRes = _registry.get<ecs::components::NetworkId>();
        if (!transformRes || !typeRes || !netRes) {
            return;
        }

        auto &transforms = transformRes->get();
        auto &types = typeRes->get();
        auto &nets = netRes->get();
        if (!transforms.has(entity) ||
            !types.has(entity) ||
            !nets.has(entity)) {
            return;
        }

        const auto &transform = transforms[entity];
        const auto &type = types[entity];
        const auto &net = nets[entity];

        auto room = _roomSystem.getRoom(roomId);
        if (!room) {
            _registry.kill(entity);
            return;
        }

        const auto players = room->getPlayers();
        if (players.empty()) {
            _registry.kill(entity);
            return;
        }

        net::Packet packet(net::OpCode::EntityDeath);
        net::EntityDeathPayload payload{};
        payload.netId = net.id;
        payload.type = static_cast<uint8_t>(type.type);
        payload.position = transform.position;
        packet << payload;

        for (const auto &player : players) {
            _networkSync.sendPacketToSession(player->getId(), packet,
                                             net::NetworkMode::TCP);
        }

        _registry.kill(entity);
    }
}
