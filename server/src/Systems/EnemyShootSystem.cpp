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

    EnemyShootSystem::EnemyShootSystem(rtp::ecs::Registry& registry,
                                       RoomSystem& roomSystem,
                                       NetworkSyncSystem& networkSync)
        : _registry(registry), _roomSystem(roomSystem), _networkSync(networkSync)
    {
    }

    void EnemyShootSystem::update(float dt)
    {
        std::vector<std::pair<rtp::ecs::components::Transform,
                              rtp::ecs::components::RoomId>> pendingSpawns;

        auto view =
            _registry.zipView<rtp::ecs::components::Transform,
                              rtp::ecs::components::EntityType,
                              rtp::ecs::components::RoomId,
                              rtp::ecs::components::SimpleWeapon>();

        for (auto &&[tf, type, roomId, weapon] : view) {
            if (type.type != rtp::net::EntityType::Scout &&
                type.type != rtp::net::EntityType::Tank &&
                type.type != rtp::net::EntityType::Boss) {
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
        const rtp::ecs::components::Transform& tf,
        const rtp::ecs::components::RoomId& roomId)
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            rtp::log::error("Failed to spawn enemy bullet entity: {}", entityRes.error().message());
            return;
        }

        rtp::ecs::Entity bullet = entityRes.value();

        const float x = tf.position.x + _spawnOffsetX;
        const float y = tf.position.y;

        _registry.addComponent<rtp::ecs::components::Transform>(
            bullet,
            rtp::ecs::components::Transform{ {x, y}, 0.f, {1.f, 1.f} }
        );

        _registry.addComponent<rtp::ecs::components::Velocity>(
            bullet,
            rtp::ecs::components::Velocity{ {_bulletSpeed, 0.f}, 0.f }
        );

        _registry.addComponent<rtp::ecs::components::BoundingBox>(
            bullet,
            rtp::ecs::components::BoundingBox{ 8.0f, 4.0f }
        );

        _registry.addComponent<rtp::ecs::components::Damage>(
            bullet,
            rtp::ecs::components::Damage{ 10, rtp::ecs::NullEntity }
        );

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            bullet,
            rtp::ecs::components::NetworkId{ static_cast<uint32_t>(bullet.index()) }
        );

        _registry.addComponent<rtp::ecs::components::EntityType>(
            bullet,
            rtp::ecs::components::EntityType{ rtp::net::EntityType::EnemyBullet }
        );

        _registry.addComponent<rtp::ecs::components::RoomId>(
            bullet,
            rtp::ecs::components::RoomId{ roomId.id }
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

        rtp::net::Packet packet(rtp::net::OpCode::EntitySpawn);
        rtp::net::EntitySpawnPayload payload = {
            static_cast<uint32_t>(bullet.index()),
            static_cast<uint8_t>(rtp::net::EntityType::EnemyBullet),
            x,
            y
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, rtp::net::NetworkMode::TCP);
    }
} // namespace rtp::server
