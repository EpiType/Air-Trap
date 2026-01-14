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

#include <algorithm>
#include <tuple>
#include <vector>

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    PlayerShootSystem::PlayerShootSystem(ecs::Registry& registry,
                                         RoomSystem& roomSystem,
                                         NetworkSyncSystem& networkSync)
        : _registry(registry), _roomSystem(roomSystem), _networkSync(networkSync)
    {
    }

    void PlayerShootSystem::update(float dt)
    {
        std::vector<std::pair<ecs::components::Transform,
                              ecs::components::RoomId>> pendingSpawns;
        std::vector<std::tuple<ecs::components::Transform,
                               ecs::components::RoomId,
                               float>> pendingChargedSpawns;

        constexpr float kChargeMax = 2.0f;
        constexpr float kChargeMin = 0.2f;

        auto view =
            _registry.zipView<ecs::components::Transform,
                              ecs::components::server::InputComponent,
                              ecs::components::EntityType,
                              ecs::components::RoomId,
                              ecs::components::SimpleWeapon,
                              ecs::components::NetworkId,
                              ecs::components::Ammo>();

        for (auto &&[tf, input, type, roomId, weapon, net, ammo] : view) {
            if (type.type != net::EntityType::Player)
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
                ecs::components::server::InputComponent::InputBits;

            if ((input.mask & Bits::Reload) && !ammo.isReloading && ammo.current < ammo.max) {
                ammo.isReloading = true;
                ammo.reloadTimer = 0.0f;
                ammo.dirty = true;
            }

            const bool shootPressed = (input.mask & Bits::Shoot);
            const bool shootWasPressed = (input.lastMask & Bits::Shoot);

            if (ammo.isReloading || ammo.current == 0) {
                input.chargeTime = 0.0f;
            }

            if (shootPressed && !ammo.isReloading && ammo.current > 0) {
                input.chargeTime = std::min(input.chargeTime + dt, kChargeMax);
            }

            if (!shootPressed && shootWasPressed) {
                if (!ammo.isReloading && ammo.current > 0) {
                    const float fireInterval = (weapon.fireRate > 0.0f)
                        ? (1.0f / weapon.fireRate)
                        : 0.0f;

                    if (input.chargeTime >= kChargeMin) {
                        const float ratio = std::clamp(input.chargeTime / kChargeMax, 0.0f, 1.0f);
                        pendingChargedSpawns.emplace_back(tf, roomId, ratio);
                        ammo.current -= 1;
                        ammo.dirty = true;
                        weapon.lastShotTime = 0.0f;
                    } else if (weapon.lastShotTime >= fireInterval) {
                        pendingSpawns.emplace_back(tf, roomId);
                        ammo.current -= 1;
                        ammo.dirty = true;
                        weapon.lastShotTime = 0.0f;
                    }
                }
                input.chargeTime = 0.0f;
            }

            if (!shootPressed && !shootWasPressed) {
                input.chargeTime = 0.0f;
            }

            input.lastMask = input.mask;

            if (ammo.dirty) {
                sendAmmoUpdate(net.id, ammo);
                ammo.dirty = false;
            }
        }

        for (const auto& [tf, roomId] : pendingSpawns) {
            spawnBullet(tf, roomId);
        }

        for (const auto& [tf, roomId, ratio] : pendingChargedSpawns) {
            spawnChargedBullet(tf, roomId, ratio);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void PlayerShootSystem::spawnBullet(
        const ecs::components::Transform& tf,
        const ecs::components::RoomId& roomId)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn bullet entity: {}", entityRes.error().message());
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
            ecs::components::Damage{ 25, ecs::NullEntity }
        );

        _registry.add<ecs::components::NetworkId>(
            bullet,
            ecs::components::NetworkId{ static_cast<uint32_t>(bullet.index()) }
        );

        _registry.add<ecs::components::EntityType>(
            bullet,
            ecs::components::EntityType{ net::EntityType::Bullet }
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
            static_cast<uint8_t>(net::EntityType::Bullet),
            x,
            y
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, net::NetworkMode::TCP);
    }

    void PlayerShootSystem::spawnChargedBullet(
        const ecs::components::Transform& tf,
        const ecs::components::RoomId& roomId,
        float chargeRatio)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn charged bullet entity: {}", entityRes.error().message());
            return;
        }

        ecs::Entity bullet = entityRes.value();

        const float x = tf.position.x + _spawnOffsetX;
        const float y = tf.position.y;
        const float ratio = std::clamp(chargeRatio, 0.0f, 1.0f);

        const float scale = (ratio < 0.34f) ? 0.5f : (ratio < 0.67f ? 1.0f : 2.0f);
        const float sizeX = 8.0f * scale;
        const float sizeY = 4.0f * scale;
        const int minDamage = 25;
        const int maxDamage = 120;
        const int damage = static_cast<int>(minDamage + (maxDamage - minDamage) * ratio);

        _registry.add<ecs::components::Transform>(
            bullet,
            ecs::components::Transform{ {x, y}, 0.f, {1.f, 1.f} }
        );

        _registry.add<ecs::components::Velocity>(
            bullet,
            ecs::components::Velocity{ {_chargedBulletSpeed, 0.f}, 0.f }
        );

        _registry.add<ecs::components::BoundingBox>(
            bullet,
            ecs::components::BoundingBox{ sizeX, sizeY }
        );

        _registry.add<ecs::components::Damage>(
            bullet,
            ecs::components::Damage{ damage, ecs::NullEntity }
        );

        _registry.add<ecs::components::NetworkId>(
            bullet,
            ecs::components::NetworkId{ static_cast<uint32_t>(bullet.index()) }
        );

        _registry.add<ecs::components::EntityType>(
            bullet,
            ecs::components::EntityType{ net::EntityType::ChargedBullet }
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
            static_cast<uint8_t>(net::EntityType::ChargedBullet),
            x,
            y,
            sizeX,
            sizeY
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, net::NetworkMode::TCP);
    }

    void PlayerShootSystem::sendAmmoUpdate(uint32_t netId, const ecs::components::Ammo& ammo)
    {
        net::Packet packet(net::OpCode::AmmoUpdate);
        net::AmmoUpdatePayload payload{};
        payload.current = ammo.current;
        payload.max = ammo.max;
        payload.isReloading = static_cast<uint8_t>(ammo.isReloading ? 1 : 0);
        payload.cooldownRemaining = ammo.isReloading
            ? (ammo.reloadCooldown - ammo.reloadTimer)
            : 0.0f;
        packet << payload;
        _networkSync.sendPacketToEntity(netId, packet, net::NetworkMode::TCP);
    }
} // namespace rtp::server
