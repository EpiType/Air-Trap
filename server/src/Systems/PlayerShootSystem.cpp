/**
 * File   : PlayerShootSystem.cpp
 * License: MIT
 * Author : Elias
 * Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   :
 * 11/12/2025
 */

#include "Systems/PlayerShootSystem.hpp"
#include "RType/Logger.hpp"

#include <vector>

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    PlayerShootSystem::PlayerShootSystem(rtp::ecs::Registry& registry,
                                         RoomSystem& roomSystem,
                                         NetworkSyncSystem& networkSync)
        : _registry(registry), _roomSystem(roomSystem), _networkSync(networkSync)
    {
    }

    void PlayerShootSystem::update(float dt)
    {
        auto view =
            _registry.zipView<rtp::ecs::components::Transform,
                              rtp::ecs::components::server::InputComponent,
                              rtp::ecs::components::EntityType,
                              rtp::ecs::components::RoomId,
                              rtp::ecs::components::SimpleWeapon,
                              rtp::ecs::components::NetworkId,
                              rtp::ecs::components::Ammo>();

        for (auto &&[tf, input, type, roomId, weapon, net, ammo] : view) {
            if (type.type != rtp::net::EntityType::Player)
                continue;

            weapon.lastShotTime += dt;
            if (ammo.isReloading) {
                ammo.reloadTimer += dt;
                if (ammo.reloadTimer >= ammo.reloadCooldown) {
                    ammo.current = ammo.max;
                    ammo.isReloading = false;
                    ammo.reloadTimer = 0.0f;
                    ammo.dirty = true;
                }
            }

            using Bits =
                rtp::ecs::components::server::InputComponent::InputBits;

            if ((input.mask & Bits::Reload) && !ammo.isReloading && ammo.current < ammo.max) {
                ammo.isReloading = true;
                ammo.reloadTimer = 0.0f;
                ammo.dirty = true;
            }

            if ((input.mask & Bits::Shoot) && !ammo.isReloading && ammo.current > 0) {
                const float fireInterval = (weapon.fireRate > 0.0f)
                    ? (1.0f / weapon.fireRate)
                    : 0.0f;

                if (weapon.lastShotTime >= fireInterval) {
                    weapon.lastShotTime = 0.0f;
                    spawnBullet(tf, roomId);
                    if (ammo.current > 0) {
                        ammo.current -= 1;
                        ammo.dirty = true;
                    }
                }
            }

            if (ammo.dirty) {
                sendAmmoUpdate(net.id, ammo);
                ammo.dirty = false;
            }
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////////////////////////////////

    void PlayerShootSystem::spawnBullet(
        const rtp::ecs::components::Transform& tf,
        const rtp::ecs::components::RoomId& roomId)
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            rtp::log::error("Failed to spawn bullet entity: {}", entityRes.error().message());
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

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            bullet,
            rtp::ecs::components::NetworkId{ static_cast<uint32_t>(bullet.index()) }
        );

        _registry.addComponent<rtp::ecs::components::EntityType>(
            bullet,
            rtp::ecs::components::EntityType{ rtp::net::EntityType::Bullet }
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
            static_cast<uint8_t>(rtp::net::EntityType::Bullet),
            x,
            y
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, rtp::net::NetworkMode::TCP);
    }

    void PlayerShootSystem::sendAmmoUpdate(uint32_t netId, const rtp::ecs::components::Ammo& ammo)
    {
        rtp::net::Packet packet(rtp::net::OpCode::AmmoUpdate);
        rtp::net::AmmoUpdatePayload payload{};
        payload.current = ammo.current;
        payload.max = ammo.max;
        payload.isReloading = static_cast<uint8_t>(ammo.isReloading ? 1 : 0);
        payload.cooldownRemaining = ammo.isReloading
            ? (ammo.reloadCooldown - ammo.reloadTimer)
            : 0.0f;
        packet << payload;
        _networkSync.sendPacketToEntity(netId, packet, rtp::net::NetworkMode::TCP);
    }
} // namespace rtp::server
