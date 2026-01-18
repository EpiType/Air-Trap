/**
 * File   : EnemyShootSystem.cpp
 * License: MIT
 * Author : Elias
 * Date   : 11/12/2025
 */

#include "Systems/EnemyShootSystem.hpp"
#include "RType/Logger.hpp"
#include "RType/ECS/Components/Boomerang.hpp"

#include <vector>

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    EnemyShootSystem::EnemyShootSystem(ecs::Registry& registry,
                                       RoomSystem& roomSystem,
                                       NetworkSyncSystem& networkSync)
        : _registry(registry), _roomSystem(roomSystem), _networkSync(networkSync)
    {
    }

    void EnemyShootSystem::update(float dt)
    {
        // Tuple: (transform, roomId, isBoomerang, shooterIndex)
        std::vector<std::tuple<ecs::components::Transform,
                              ecs::components::RoomId,
                              bool,
                              uint32_t>> pendingSpawns;

        auto view =
            _registry.zipView<ecs::components::Transform,
                              ecs::components::EntityType,
                              ecs::components::RoomId,
                              ecs::components::SimpleWeapon>();

        for (auto &&[tf, type, roomId, weapon] : view) {
            if (type.type != net::EntityType::Scout &&
                type.type != net::EntityType::Tank &&
                type.type != net::EntityType::Boss &&
                type.type != net::EntityType::Boss2 &&
                type.type != net::EntityType::BossShield) {
                continue;
            }

            if (weapon.fireRate <= 0.0f) {
                continue;
            }

            weapon.lastShotTime += dt;
            const float fireInterval = (1.0f / weapon.fireRate);

            if (weapon.lastShotTime >= fireInterval) {
                weapon.lastShotTime = 0.0f;
                // Boss2 uses boomerang projectiles
                bool isBoomerang = (type.type == net::EntityType::Boss2);
                // Get entity index for boomerang tracking - using a simple position hash
                uint32_t shooterIdx = static_cast<uint32_t>(tf.position.x * 1000 + tf.position.y);
                pendingSpawns.emplace_back(tf, roomId, isBoomerang, shooterIdx);
            }
        }

        for (const auto& [tf, roomId, isBoomerang, shooterIdx] : pendingSpawns) {
            spawnBullet(tf, roomId, isBoomerang, shooterIdx);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void EnemyShootSystem::spawnBullet(
        const ecs::components::Transform& tf,
        const ecs::components::RoomId& roomId,
        bool isBoomerang,
        uint32_t shooterIndex)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn enemy bullet entity: {}", entityRes.error().message());
            return;
        }

        ecs::Entity bullet = entityRes.value();

        const float x = tf.position.x + _spawnOffsetX;
        const float y = tf.position.y;

        _registry.add<ecs::components::Transform>(
            bullet,
            ecs::components::Transform{ {x, y}, 0.f, {1.f, 1.f} }
        );

        // Boomerang bullets go slower and curve back
        float bulletSpeed = isBoomerang ? -200.0f : _bulletSpeed;
        _registry.add<ecs::components::Velocity>(
            bullet,
            ecs::components::Velocity{ {bulletSpeed, 0.f}, 0.f }
        );

        // Larger hitbox for boomerang
        float bboxW = isBoomerang ? 24.0f : 8.0f;
        float bboxH = isBoomerang ? 24.0f : 4.0f;
        _registry.add<ecs::components::BoundingBox>(
            bullet,
            ecs::components::BoundingBox{ bboxW, bboxH }
        );

        // More damage for boomerang
        int damage = isBoomerang ? 25 : 10;
        _registry.add<ecs::components::Damage>(
            bullet,
            ecs::components::Damage{ damage, ecs::NullEntity }
        );

        _registry.add<ecs::components::NetworkId>(
            bullet,
            ecs::components::NetworkId{ static_cast<uint32_t>(bullet.index()) }
        );

        net::EntityType bulletType = isBoomerang ? net::EntityType::Boss2Bullet : net::EntityType::EnemyBullet;
        _registry.add<ecs::components::EntityType>(
            bullet,
            ecs::components::EntityType{ bulletType }
        );

        _registry.add<ecs::components::RoomId>(
            bullet,
            ecs::components::RoomId{ roomId.id }
        );

        // Add boomerang component for Boss2 bullets
        if (isBoomerang) {
            ecs::components::Boomerang boom;
            boom.ownerIndex = shooterIndex;
            boom.startPos = {x, y};
            boom.maxDistance = 500.0f;  // Travel distance before returning
            boom.returning = false;
            _registry.add<ecs::components::Boomerang>(bullet, boom);
        }

        auto room = _roomSystem.getRoom(roomId.id);
        if (!room)
            return;
        if (room->getState() != Room::State::InGame)
            return;

        const auto players = room->getPlayers();
        std::vector<uint32_t> sessions;
        sessions.reserve(players.size());
        for (const auto& player : players) {
            sessions.push_back(player->getId());
        }

        net::Packet packet(net::OpCode::EntitySpawn);
        net::EntitySpawnPayload payload = {
            static_cast<uint32_t>(bullet.index()),
            static_cast<uint8_t>(bulletType),
            x,
            y
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, net::NetworkMode::TCP);
    }
} // namespace rtp::server
