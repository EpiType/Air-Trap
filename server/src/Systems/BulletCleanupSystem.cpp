/**
 * File   : BulletCleanupSystem.cpp
 * License: MIT
 * Author : Elias
 * Date   : 11/12/2025
 */

#include "Systems/BulletCleanupSystem.hpp"

namespace rtp::server
{
    BulletCleanupSystem::BulletCleanupSystem(rtp::ecs::Registry& registry,
                                             RoomSystem& roomSystem,
                                             NetworkSyncSystem& networkSync)
        : _registry(registry), _roomSystem(roomSystem), _networkSync(networkSync)
    {
    }

    void BulletCleanupSystem::despawn(const rtp::ecs::Entity& entity, uint32_t roomId)
    {
        auto transformRes = _registry.get<rtp::ecs::components::Transform>();
        auto typeRes = _registry.get<rtp::ecs::components::EntityType>();
        auto netRes = _registry.get<rtp::ecs::components::NetworkId>();
        if (!transformRes || !typeRes || !netRes) {
            return;
        }

        auto &transforms = transformRes->get();
        auto &types = typeRes->get();
        auto &nets = netRes->get();
        if (!transforms.has(entity) || !types.has(entity) || !nets.has(entity)) {
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

        rtp::net::Packet packet(rtp::net::OpCode::EntityDeath);
        rtp::net::EntityDeathPayload payload{};
        payload.netId = net.id;
        payload.type = static_cast<uint8_t>(type.type);
        payload.position = transform.position;
        packet << payload;

        for (const auto& player : players) {
            _networkSync.sendPacketToSession(player->getId(), packet, rtp::net::NetworkMode::TCP);
        }

        _registry.kill(entity);
    }

    void BulletCleanupSystem::update(float dt)
    {
        (void)dt;
        auto transformsRes = _registry.get<rtp::ecs::components::Transform>();
        auto typesRes = _registry.get<rtp::ecs::components::EntityType>();
        auto roomsRes = _registry.get<rtp::ecs::components::RoomId>();

        if (!transformsRes || !typesRes || !roomsRes) {
            return;
        }

        auto &transforms = transformsRes->get();
        auto &types = typesRes->get();
        auto &rooms = roomsRes->get();

        std::vector<std::pair<rtp::ecs::Entity, uint32_t>> pending;

        for (auto entity : types.entities()) {
            if (!types.has(entity)) {
                continue;
            }
            const auto etype = types[entity].type;
            if (etype != rtp::net::EntityType::Bullet &&
                etype != rtp::net::EntityType::ChargedBullet &&
                etype != rtp::net::EntityType::EnemyBullet &&
                etype != rtp::net::EntityType::Scout &&
                etype != rtp::net::EntityType::Tank &&
                etype != rtp::net::EntityType::Boss &&
                etype != rtp::net::EntityType::PowerupHeal &&
                etype != rtp::net::EntityType::PowerupSpeed) {
                continue;
            }
            if (!transforms.has(entity) || !rooms.has(entity)) {
                continue;
            }

            const auto &tf = transforms[entity];
            if (tf.position.x < _minX || tf.position.x > _maxX) {
                pending.emplace_back(entity, rooms[entity].id);
            }
        }

        for (const auto& [entity, roomId] : pending) {
            despawn(entity, roomId);
        }
    }
} // namespace rtp::server
