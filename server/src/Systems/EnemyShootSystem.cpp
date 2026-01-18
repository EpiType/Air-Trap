/**
 * File   : EnemyShootSystem.cpp
 * License: MIT
 * Author : Elias
 * Date   : 11/12/2025
 */

#include "Systems/EnemyShootSystem.hpp"
#include "RType/Logger.hpp"

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
        std::vector<std::pair<ecs::components::Transform,
                              ecs::components::RoomId>> pendingSpawns;

        auto view =
            _registry.zipView<ecs::components::Transform,
                              ecs::components::EntityType,
                              ecs::components::RoomId,
                              ecs::components::SimpleWeapon>();

        for (auto &&[tf, type, roomId, weapon] : view) {
            if (type.type != net::EntityType::Scout &&
                type.type != net::EntityType::Tank &&
                type.type != net::EntityType::Boss &&
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
                pendingSpawns.emplace_back(tf, roomId);
            }
        }

        for (const auto& [tf, roomId] : pendingSpawns) {
            spawnBullet(tf, roomId);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void EnemyShootSystem::spawnBullet(
        const ecs::components::Transform& tf,
        const ecs::components::RoomId& roomId)
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

        _registry.add<ecs::components::Velocity>(
            bullet,
            ecs::components::Velocity{ {_bulletSpeed, 0.f}, 0.f }
        );

        _registry.add<ecs::components::BoundingBox>(
            bullet,
            ecs::components::BoundingBox{ 8.0f, 4.0f }
        );

        _registry.add<ecs::components::Damage>(
            bullet,
            ecs::components::Damage{ 10, ecs::NullEntity }
        );

        _registry.add<ecs::components::NetworkId>(
            bullet,
            ecs::components::NetworkId{ static_cast<uint32_t>(bullet.index()) }
        );

        _registry.add<ecs::components::EntityType>(
            bullet,
            ecs::components::EntityType{ net::EntityType::EnemyBullet }
        );

        _registry.add<ecs::components::RoomId>(
            bullet,
            ecs::components::RoomId{ roomId.id }
        );

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
            static_cast<uint8_t>(net::EntityType::EnemyBullet),
            x,
            y
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, net::NetworkMode::TCP);
    }
} // namespace rtp::server
