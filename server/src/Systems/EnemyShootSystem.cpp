/**
 * File   : EnemyShootSystem.cpp
 * License: MIT
 * Author : Elias
 * Date   : 11/12/2025
 */

#include "Systems/EnemyShootSystem.hpp"
#include "RType/Logger.hpp"
#include "RType/ECS/Components/Lifetime.hpp"

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
        std::vector<std::tuple<ecs::components::Transform,
                              ecs::components::RoomId,
                              net::EntityType>> pendingSpawns;

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

            // Boss2 can't shoot if there's already a beam in the room
            if (type.type == net::EntityType::Boss2) {
                if (hasBeamInRoom(roomId.id)) {
                    log::debug("Boss2 cannot shoot: beam already exists in room {}", roomId.id);
                    continue;
                } else {
                    log::debug("Boss2 ready to shoot in room {}", roomId.id);
                }
            }

            weapon.lastShotTime += dt;
            const float fireInterval = (1.0f / weapon.fireRate);

            if (weapon.lastShotTime >= fireInterval) {
                weapon.lastShotTime = 0.0f;
                pendingSpawns.emplace_back(tf, roomId, type.type);
            }
        }

        for (const auto& [tf, roomId, shooterType] : pendingSpawns) {
            spawnBullet(tf, roomId, shooterType);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void EnemyShootSystem::spawnBullet(
        const ecs::components::Transform& tf,
        const ecs::components::RoomId& roomId,
        net::EntityType shooterType)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn enemy bullet entity: {}", entityRes.error().message());
            return;
        }

        ecs::Entity bullet = entityRes.value();

        const bool isBoss2 = (shooterType == net::EntityType::Boss2);
        // Boss2 beam spawns at the boss front (left edge of the boss sprite)
        // With client pivot at beam right-center, set position to boss's left edge
        // Boss2 bbox width ≈ 536px, so front is ~ -268px from boss center
        const float offsetX = isBoss2 ? -268.0f : _spawnOffsetX;
        const float x = tf.position.x + offsetX;
        float y = tf.position.y;
        
        // For Boss2 beam, spawn at the height of the nearest player
        if (isBoss2) {
            y = getNearestPlayerY(x, tf.position.y, roomId.id);
        }

        _registry.add<ecs::components::Transform>(
            bullet,
            ecs::components::Transform{ {x, y}, 0.f, {1.f, 1.f} }
        );

        // BossBeam stays in place, other bullets move left
        _registry.add<ecs::components::Velocity>(
            bullet,
            ecs::components::Velocity{ {isBoss2 ? 0.f : _bulletSpeed, 0.f}, 0.f }
        );

        // Boss2 beam has much larger hitbox
        _registry.add<ecs::components::BoundingBox>(
            bullet,
            ecs::components::BoundingBox{ isBoss2 ? 1900.0f : 8.0f, isBoss2 ? 351.0f : 4.0f }
        );

        // Boss2 beam deals more damage
        _registry.add<ecs::components::Damage>(
            bullet,
            ecs::components::Damage{ isBoss2 ? 30 : 10, ecs::NullEntity }
        );

        _registry.add<ecs::components::NetworkId>(
            bullet,
            ecs::components::NetworkId{ static_cast<uint32_t>(bullet.index()) }
        );

        const auto bulletType = isBoss2 ? net::EntityType::BossBeam : net::EntityType::EnemyBullet;
        _registry.add<ecs::components::EntityType>(
            bullet,
            ecs::components::EntityType{ bulletType }
        );

        _registry.add<ecs::components::RoomId>(
            bullet,
            ecs::components::RoomId{ roomId.id }
        );

        // BossBeam has limited lifetime (2 seconds)
        if (isBoss2) {
            _registry.add<ecs::components::Lifetime>(
                bullet,
                ecs::components::Lifetime{ 2.0f, 0.0f }
            );
            // BeamTarget removed: no continuous tracking
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

    bool EnemyShootSystem::hasBeamInRoom(uint32_t roomId) const
    {
        auto typesRes = _registry.get<ecs::components::EntityType>();
        auto roomsRes = _registry.get<ecs::components::RoomId>();

        if (!typesRes || !roomsRes) {
            return false;
        }

        auto& types = typesRes->get();
        auto& rooms = roomsRes->get();

        for (auto entity : types.entities()) {
            if (!types.has(entity) || !rooms.has(entity)) {
                continue;
            }
            if (types[entity].type == net::EntityType::BossBeam &&
                rooms[entity].id == roomId) {
                return true;
            }
        }
        return false;
    }

    float EnemyShootSystem::getNearestPlayerY(float bossX, float bossY, uint32_t roomId) const
    {
        auto room = _roomSystem.getRoom(roomId);
        if (!room) {
            return bossY;
        }

        // Get player entities from registry
        auto transformsRes = _registry.get<ecs::components::Transform>();
        auto typesRes = _registry.get<ecs::components::EntityType>();
        auto roomsRes = _registry.get<ecs::components::RoomId>();

        if (!transformsRes || !typesRes || !roomsRes) {
            return bossY;
        }

        auto& transforms = transformsRes->get();
        auto& types = typesRes->get();
        auto& rooms = roomsRes->get();

        // Find nearest player in this room
        float minDist = std::numeric_limits<float>::max();
        float nearestPlayerY = bossY;

        for (auto entity : types.entities()) {
            if (!types.has(entity) || !transforms.has(entity) || !rooms.has(entity)) {
                continue;
            }
            if (types[entity].type == net::EntityType::Player &&
                rooms[entity].id == roomId) {
                const float playerX = transforms[entity].position.x;
                const float playerY = transforms[entity].position.y;
                const float dist = std::hypot(playerX - bossX, playerY - bossY);
                if (dist < minDist) {
                    minDist = dist;
                    nearestPlayerY = playerY;
                }
            }
        }

        return nearestPlayerY;
    }
} // namespace rtp::server
