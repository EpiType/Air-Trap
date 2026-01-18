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
#include "RType/ECS/Components/Powerup.hpp"

#include <algorithm>
#include <tuple>
#include <vector>
#include <cstdlib>

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
        std::vector<std::tuple<ecs::components::Transform,
                              ecs::components::RoomId,
                              bool>> pendingSpawns; // Added bool for doubleFire
        std::vector<std::tuple<ecs::components::Transform,
                               ecs::components::RoomId,
                               float,
                               bool>> pendingChargedSpawns; // Added bool for doubleFire

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
        
        auto doubleFireRes = _registry.get<ecs::components::DoubleFire>();
        
        // Get all component arrays
        auto transformRes = _registry.get<ecs::components::Transform>();
        auto inputRes = _registry.get<ecs::components::server::InputComponent>();
        auto typeRes = _registry.get<ecs::components::EntityType>();
        auto roomIdRes = _registry.get<ecs::components::RoomId>();
        auto weaponRes = _registry.get<ecs::components::SimpleWeapon>();
        auto netIdRes = _registry.get<ecs::components::NetworkId>();
        auto ammoRes = _registry.get<ecs::components::Ammo>();
        
        if (!transformRes || !inputRes || !typeRes || !roomIdRes || !weaponRes || !netIdRes || !ammoRes)
            return;
            
        auto& transforms = transformRes->get();
        auto& inputs = inputRes->get();
        auto& types = typeRes->get();
        auto& roomIds = roomIdRes->get();
        auto& weapons = weaponRes->get();
        auto& netIds = netIdRes->get();
        auto& ammos = ammoRes->get();
        
        for (size_t i = 0; i < transforms.size(); ++i) {
            ecs::Entity entity{static_cast<uint32_t>(i), 0};
            
            if (!transforms.has(entity) || !inputs.has(entity) || !types.has(entity) ||
                !roomIds.has(entity) || !weapons.has(entity) || !netIds.has(entity) || !ammos.has(entity))
                continue;
                
            auto& tf = transforms[entity];
            auto& input = inputs[entity];
            auto& type = types[entity];
            auto& roomId = roomIds[entity];
            auto& weapon = weapons[entity];
            auto& net = netIds[entity];
            auto& ammo = ammos[entity];
            if (type.type != net::EntityType::Player)
                return;
            
            // Update double fire timer
            bool hasDoubleFire = false;
            if (doubleFireRes) {
                auto& doubleFires = doubleFireRes->get();
                if (doubleFires.has(entity)) {
                    auto& doubleFire = doubleFires[entity];
                    doubleFire.remainingTime -= dt;
                    if (doubleFire.remainingTime <= 0.0f) {
                        _registry.remove<ecs::components::DoubleFire>(entity);
                    } else {
                        hasDoubleFire = true;
                    }
                }
            }

            weapon.lastShotTime += dt;
            
            using Bits = ecs::components::server::InputComponent::InputBits;

            // Debug: Spawn powerup on P key press
            const bool debugPressed = (input.mask & Bits::DebugPowerup);
            const bool debugWasPressed = (input.lastMask & Bits::DebugPowerup);
            
            if (debugPressed && !debugWasPressed) {
                Vec2f spawnPos = tf.position;
                spawnPos.x += 50.0f;
                
                // Spawn all 3 powerup types for testing
                spawnDebugPowerup(spawnPos, roomId.id, 0);  // Heal (red)
                spawnPos.y += 30.0f;
                spawnDebugPowerup(spawnPos, roomId.id, 15); // DoubleFire (yellow)
                spawnPos.y += 30.0f;
                spawnDebugPowerup(spawnPos, roomId.id, 25); // Shield (green)
                
                log::info("[DEBUG] Spawned all 3 powerup types at player position");
            }
            
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

            // Debug: Spawn powerup on P key
            if ((input.mask & Bits::DebugPowerup) && !(input.lastMask & Bits::DebugPowerup)) {
                // Spawn a random powerup at player position
                const int debugRoll = std::rand() % 30;  // 0-29 for all powerup types
                Vec2f spawnPos = tf.position;
                spawnPos.x += 50.0f;  // Spawn slightly ahead of player
                spawnDebugPowerup(spawnPos, roomId.id, debugRoll);
                log::info("[DEBUG] Spawned powerup at player position (roll: {})", debugRoll);
            }

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
                        pendingChargedSpawns.emplace_back(tf, roomId, ratio, hasDoubleFire);
                        ammo.current -= 1;
                        ammo.dirty = true;
                        weapon.lastShotTime = 0.0f;
                    } else if (weapon.lastShotTime >= fireInterval) {
                        pendingSpawns.emplace_back(tf, roomId, hasDoubleFire);
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

        for (const auto& [tf, roomId, doubleFire] : pendingSpawns) {
            spawnBullet(tf, roomId, doubleFire);
        }

        for (const auto& [tf, roomId, ratio, doubleFire] : pendingChargedSpawns) {
            spawnChargedBullet(tf, roomId, ratio, doubleFire);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void PlayerShootSystem::spawnBullet(
        const ecs::components::Transform& tf,
        const ecs::components::RoomId& roomId,
        bool doubleFire)
    {
        // Spawn first bullet
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn bullet entity: {}", entityRes.error().message());
            return;
        }

        ecs::Entity bullet = entityRes.value();

        const float x = tf.position.x + _spawnOffsetX;
        float y = tf.position.y;
        
        // If double fire, offset vertically
        if (doubleFire) {
            y -= 4.0f; // Offset first bullet up by 4 pixels
        }

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
            , 0.0f, 0.0f, 0
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, net::NetworkMode::TCP);
        
        // Spawn second bullet if double fire is active
        if (doubleFire) {
            auto entityRes2 = _registry.spawn();
            if (!entityRes2) {
                log::error("Failed to spawn second bullet entity: {}", entityRes2.error().message());
                return;
            }

            ecs::Entity bullet2 = entityRes2.value();
            const float y2 = tf.position.y + 4.0f; // Offset second bullet down by 4 pixels

            _registry.add<ecs::components::Transform>(
                bullet2,
                ecs::components::Transform{ {x, y2}, 0.f, {1.f, 1.f} }
            );

            _registry.add<ecs::components::Velocity>(
                bullet2,
                ecs::components::Velocity{ {_bulletSpeed, 0.f}, 0.f }
            );

            _registry.add<ecs::components::BoundingBox>(
                bullet2,
                ecs::components::BoundingBox{ 8.0f, 4.0f }
            );

            _registry.add<ecs::components::Damage>(
                bullet2,
                ecs::components::Damage{ 25, ecs::NullEntity }
            );

            _registry.add<ecs::components::NetworkId>(
                bullet2,
                ecs::components::NetworkId{ static_cast<uint32_t>(bullet2.index()) }
            );

            _registry.add<ecs::components::EntityType>(
                bullet2,
                ecs::components::EntityType{ net::EntityType::Bullet }
            );

            _registry.add<ecs::components::RoomId>(
                bullet2,
                ecs::components::RoomId{ roomId.id }
            );

            net::Packet packet2(net::OpCode::EntitySpawn);
            net::EntitySpawnPayload payload2 = {
                static_cast<uint32_t>(bullet2.index()),
                static_cast<uint8_t>(net::EntityType::Bullet),
                x,
                y2
            };
            packet2 << payload2;
            _networkSync.sendPacketToSessions(sessions, packet2, net::NetworkMode::TCP);
        }
    }

    void PlayerShootSystem::spawnChargedBullet(
        const ecs::components::Transform& tf,
        const ecs::components::RoomId& roomId,
        float chargeRatio,
        bool doubleFire)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn charged bullet entity: {}", entityRes.error().message());
            return;
        }

        ecs::Entity bullet = entityRes.value();

        const float x = tf.position.x + _spawnOffsetX;
        float y = tf.position.y;
        
        // If double fire, offset vertically
        if (doubleFire) {
            y -= 4.0f; // Offset first bullet up by 4 pixels
        }
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
        
        // Spawn second charged bullet if double fire is active
        if (doubleFire) {
            auto entityRes2 = _registry.spawn();
            if (!entityRes2) {
                log::error("Failed to spawn second charged bullet entity: {}", entityRes2.error().message());
                return;
            }

            ecs::Entity bullet2 = entityRes2.value();
            const float y2 = tf.position.y + 4.0f; // Offset second bullet down by 4 pixels

            _registry.add<ecs::components::Transform>(
                bullet2,
                ecs::components::Transform{ {x, y2}, 0.f, {1.f, 1.f} }
            );

            _registry.add<ecs::components::Velocity>(
                bullet2,
                ecs::components::Velocity{ {_chargedBulletSpeed, 0.f}, 0.f }
            );

            _registry.add<ecs::components::BoundingBox>(
                bullet2,
                ecs::components::BoundingBox{ sizeX, sizeY }
            );

            _registry.add<ecs::components::Damage>(
                bullet2,
                ecs::components::Damage{ damage, ecs::NullEntity }
            );

            _registry.add<ecs::components::NetworkId>(
                bullet2,
                ecs::components::NetworkId{ static_cast<uint32_t>(bullet2.index()) }
            );

            _registry.add<ecs::components::EntityType>(
                bullet2,
                ecs::components::EntityType{ net::EntityType::ChargedBullet }
            );

            _registry.add<ecs::components::RoomId>(
                bullet2,
                ecs::components::RoomId{ roomId.id }
            );

            net::Packet packet2(net::OpCode::EntitySpawn);
            net::EntitySpawnPayload payload2 = {
                static_cast<uint32_t>(bullet2.index()),
                static_cast<uint8_t>(net::EntityType::ChargedBullet),
                x,
                y2,
                sizeX,
                sizeY
            };
            packet2 << payload2;
            _networkSync.sendPacketToSessions(sessions, packet2, net::NetworkMode::TCP);
        }
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

    void PlayerShootSystem::spawnDebugPowerup(const Vec2f& position, uint32_t roomId, int dropRoll)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn debug powerup: {}", entityRes.error().message());
            return;
        }

        ecs::Entity e = entityRes.value();

        _registry.add<ecs::components::Transform>(e, ecs::components::Transform{position, 0.0f, {1.0f, 1.0f}});
        _registry.add<ecs::components::Velocity>(e, ecs::components::Velocity{Vec2f{-1.0f, 0.0f}, 30.0f});
        _registry.add<ecs::components::BoundingBox>(e, ecs::components::BoundingBox{16.0f, 16.0f});
        _registry.add<ecs::components::RoomId>(e, ecs::components::RoomId{roomId});

        // Determine powerup type from dropRoll
        net::EntityType entityType;
        ecs::components::PowerupType powerupType;
        
        if (dropRoll < 10) {
            entityType = net::EntityType::PowerupHeal;
            powerupType = ecs::components::PowerupType::Heal;
        } else if (dropRoll < 20) {
            entityType = net::EntityType::PowerupDoubleFire;
            powerupType = ecs::components::PowerupType::DoubleFire;
        } else {
            entityType = net::EntityType::PowerupShield;
            powerupType = ecs::components::PowerupType::Shield;
        }

        _registry.add<ecs::components::EntityType>(e, ecs::components::EntityType{entityType});
        _registry.add<ecs::components::Powerup>(e, ecs::components::Powerup{powerupType, 1.0f, 0.0f});
        _registry.add<ecs::components::NetworkId>(e, ecs::components::NetworkId{static_cast<uint32_t>(e.index())});

        auto room = _roomSystem.getRoom(roomId);
        if (!room)
            return;

        const auto players = room->getPlayers();
        std::vector<uint32_t> sessions;
        sessions.reserve(players.size());
        for (const auto& player : players) {
            sessions.push_back(player->getId());
        }

        net::Packet packet(net::OpCode::EntitySpawn);
        net::EntitySpawnPayload payload = {
            static_cast<uint32_t>(e.index()),
            static_cast<uint8_t>(entityType),
            position.x,
            position.y
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, net::NetworkMode::TCP);
    }
} // namespace rtp::server
