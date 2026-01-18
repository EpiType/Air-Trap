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
#include "RType/ECS/Components/Health.hpp"
#include "RType/ECS/Components/BoundingBox.hpp"
#include "RType/Network/Packet.hpp"
#include <cmath>

#include <algorithm>
#include <tuple>
#include <vector>
#include <cstdlib>

namespace rtp::server
{
    namespace {
        int getKillScore(net::EntityType type)
        {
            switch (type) {
                case net::EntityType::Scout: return 10;
                case net::EntityType::Tank: return 25;
                case net::EntityType::Boss: return 100;
                case net::EntityType::Obstacle: return 5;
                case net::EntityType::ObstacleSolid: return 15;
                default: return 0;
            }
        }
    } // namespace

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
        std::vector<std::tuple<ecs::Entity,
                      ecs::components::Transform,
                      ecs::components::RoomId,
                      bool>> pendingSpawns; // owner, transform, roomId, doubleFire
        std::vector<std::tuple<ecs::Entity,
                       ecs::components::Transform,
                       ecs::components::RoomId,
                       float,
                       bool>> pendingChargedSpawns; // owner, transform, roomId, ratio, doubleFire

        auto updatePlayerScore = [&](uint32_t roomId, ecs::Entity owner, int delta) {
            if (delta == 0 || owner == ecs::NullEntity) {
                return;
            }
            auto room = _roomSystem.getRoom(roomId);
            if (!room) {
                return;
            }
            const auto playersInRoom = room->getPlayers();
            for (const auto &player : playersInRoom) {
                if (!player) {
                    continue;
                }
                if (player->getEntityId() != static_cast<uint32_t>(owner.index())) {
                    continue;
                }
                player->addScore(delta);
                net::Packet packet(net::OpCode::ScoreUpdate);
                net::ScoreUpdatePayload payload{player->getScore()};
                packet << payload;
                _networkSync.sendPacketToSession(player->getId(), packet, net::NetworkMode::TCP);
                break;
            }
        };

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
        auto healthRes = _registry.get<ecs::components::Health>();
        auto boxRes = _registry.get<ecs::components::BoundingBox>();
        
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
                continue;
            
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

            const bool hasInfiniteAmmo = (weapon.maxAmmo < 0);
            const bool canShoot = !ammo.isReloading &&
                (ammo.current > 0 || hasInfiniteAmmo);

            // Update beam cooldown timer
            if (weapon.beamCooldownRemaining > 0.0f) {
                weapon.beamCooldownRemaining = std::max(0.0f, weapon.beamCooldownRemaining - dt);
            }

            // Beam active handling: apply periodic damage along a horizontal line in front of the player
            if (weapon.kind == ecs::components::WeaponKind::Beam && weapon.beamActive) {
                weapon.beamActiveTime -= dt;
                // accumulate tick
                auto &acc = _beamTickTimers[static_cast<uint32_t>(entity.index())];
                acc += dt;
                constexpr float kBeamTick = 0.2f; // apply damage every 0.2s
                    if (acc >= kBeamTick) {
                    acc -= kBeamTick;
                    // apply damage to entities in same room whose x is in front of player
                    if (healthRes && typeRes && transformRes && roomIdRes) {
                        auto &healths = healthRes->get();
                        auto *boxes = boxRes ? &boxRes->get() : nullptr;
                        auto room = _roomSystem.getRoom(roomId.id);
                        if (room) {
                            const auto players = room->getPlayers();
                            std::vector<uint32_t> sessions;
                            sessions.reserve(players.size());
                            for (const auto& p : players) sessions.push_back(p->getId());
                            for (auto target : healths.entities()) {
                                ecs::Entity t = target;
                                if (!transforms.has(t) || !types.has(t) || !roomIds.has(t) || !healths.has(t))
                                    continue;
                                if (roomIds[t].id != roomId.id)
                                    continue;
                                if (types[t].type == net::EntityType::Player)
                                    continue;
                                // Only hit entities located in front of player (x greater)
                                const auto &ttf = transforms[t];
                                if (ttf.position.x <= tf.position.x)
                                    continue;
                                // Y proximity check: support single or double beam offsets
                                std::vector<float> centers;
                                centers.push_back(tf.position.y);
                                if (weapon.beamWasDouble) {
                                    centers.clear();
                                    centers.push_back(tf.position.y - 4.0f);
                                    centers.push_back(tf.position.y + 4.0f);
                                }

                                float halfH = 8.0f;
                                if (boxes && boxes->has(t)) {
                                    halfH = (*boxes)[t].height * 0.5f;
                                }

                                // For each beam center, if within vertical range apply damage
                                for (float centerY : centers) {
                                    if (std::fabs(ttf.position.y - centerY) > halfH + 4.0f)
                                        continue;

                                    // Apply damage for this beam
                                    auto &ht = healths[t];
                                    ht.currentHealth -= weapon.damage;
                                    log::info("Beam: applied {} damage to entity {} (hp left={})", weapon.damage, t.index(), ht.currentHealth);
                                    if (ht.currentHealth <= 0) {
                                        const int award = getKillScore(types[t].type);
                                        updatePlayerScore(roomId.id, entity, award);
                                        // 30% chance to drop a power-up (beam kills should drop too)
                                        const int dropChance = std::rand() % 100;
                                        if (dropChance < 30) {
                                            // spawn a powerup using this system's debug spawner
                                            spawnDebugPowerup(transforms[t].position, roomId.id, dropChance);
                                        }

                                        // send death packet to players in room
                                        net::Packet dpacket(net::OpCode::EntityDeath);
                                        net::EntityDeathPayload dp{};
                                        if (auto netRes = _registry.get<ecs::components::NetworkId>()) {
                                            auto &nets = netRes->get();
                                            if (nets.has(t)) dp.netId = nets[t].id;
                                        }
                                        dp.type = static_cast<uint8_t>(types[t].type);
                                        dp.position = transforms[t].position;
                                        dpacket << dp;
                                        _networkSync.sendPacketToSessions(sessions, dpacket, net::NetworkMode::TCP);
                                        _registry.kill(t);
                                        break; // entity dead, stop processing centers
                                    }
                                }
                            }
                        }
                    }
                }
                // If beam duration ended, stop beam and start cooldown
                if (weapon.beamActiveTime <= 0.0f) {
                    weapon.beamActive = false;
                    weapon.beamCooldownRemaining = weapon.beamCooldown;
                    _beamTickTimers.erase(static_cast<uint32_t>(entity.index()));
                    // Notify clients in the room about beam end (visual removal)
                    auto roomForEnd = _roomSystem.getRoom(roomId.id);
                    if (roomForEnd) {
                        const auto playersForEnd = roomForEnd->getPlayers();
                        std::vector<uint32_t> sessionsForEnd;
                        sessionsForEnd.reserve(playersForEnd.size());
                        for (const auto &p : playersForEnd) sessionsForEnd.push_back(p->getId());

                        // send beam end notifications. If double fire, send two end packets
                        if (weapon.beamWasDouble) {
                            net::Packet b1(net::OpCode::BeamState);
                            net::BeamStatePayload bp1{};
                            bp1.ownerNetId = net.id;
                            bp1.active = 0;
                            bp1.timeRemaining = 0.0f;
                            bp1.length = 0.0f;
                            bp1.offsetY = -4.0f;
                            b1 << bp1;
                            _networkSync.sendPacketToSessions(sessionsForEnd, b1, net::NetworkMode::TCP);

                            net::Packet b2(net::OpCode::BeamState);
                            net::BeamStatePayload bp2{};
                            bp2.ownerNetId = net.id;
                            bp2.active = 0;
                            bp2.timeRemaining = 0.0f;
                            bp2.length = 0.0f;
                            bp2.offsetY = 4.0f;
                            b2 << bp2;
                            _networkSync.sendPacketToSessions(sessionsForEnd, b2, net::NetworkMode::TCP);
                        } else {
                            net::Packet bpacket(net::OpCode::BeamState);
                            net::BeamStatePayload bp{};
                            bp.ownerNetId = net.id;
                            bp.active = 0;
                            bp.timeRemaining = 0.0f;
                            bp.length = 0.0f;
                            bp.offsetY = 0.0f;
                            bpacket << bp;
                            _networkSync.sendPacketToSessions(sessionsForEnd, bpacket, net::NetworkMode::TCP);
                        }
                    }
                    // reset captured double-fire flag after beam ended
                    weapon.beamWasDouble = false;
                }
            }
            
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
            
            // Progress reload only when beam is not active (pause reload during beam)
            if (ammo.isReloading && !weapon.beamActive) {
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

            // Do not allow starting a reload while beam is active
            if ((input.mask & Bits::Reload) && !ammo.isReloading && ammo.current < ammo.max && !weapon.beamActive) {
                ammo.isReloading = true;
                ammo.reloadTimer = 0.0f;
                ammo.dirty = true;
            }

            const bool shootPressed = (input.mask & Bits::Shoot);
            const bool shootWasPressed = (input.lastMask & Bits::Shoot);

            // Beam activation on press: if player has Beam weapon, not in cooldown and has ammo
            if (weapon.kind == ecs::components::WeaponKind::Beam && shootPressed && !shootWasPressed) {
                if (weapon.beamCooldownRemaining <= 0.0f && !weapon.beamActive && (ammo.current > 0 || hasInfiniteAmmo)) {
                    weapon.beamActive = true;
                    weapon.beamActiveTime = weapon.beamDuration;
                    // capture whether this beam instance was started with double-fire
                    weapon.beamWasDouble = hasDoubleFire;
                    // consume one ammo
                    if (!hasInfiniteAmmo && ammo.current > 0 && ammo.max > 0) {
                        ammo.current = static_cast<uint16_t>(std::max<int>(0, static_cast<int>(ammo.current) - 1));
                        ammo.dirty = true;
                        sendAmmoUpdate(net.id, ammo);
                    } else if (ammo.max == 0) {
                        // no ammo - cancel activation
                        weapon.beamActive = false;
                    }
                    // Notify clients in the room about beam start (visual)
                    {
                        auto room = _roomSystem.getRoom(roomId.id);
                        if (room) {
                            const auto players = room->getPlayers();
                            std::vector<uint32_t> sessions;
                            sessions.reserve(players.size());
                            for (const auto &p : players) sessions.push_back(p->getId());

                            // send beam start(s). If double fire is active, send two beams with vertical offsets
                            const float visualLength = 800.0f;
                            if (weapon.beamWasDouble) {
                                net::Packet bpacket1(net::OpCode::BeamState);
                                net::BeamStatePayload bp1{};
                                bp1.ownerNetId = net.id;
                                bp1.active = 1;
                                bp1.timeRemaining = weapon.beamActiveTime;
                                bp1.length = visualLength;
                                bp1.offsetY = -4.0f;
                                bpacket1 << bp1;
                                _networkSync.sendPacketToSessions(sessions, bpacket1, net::NetworkMode::TCP);

                                net::Packet bpacket2(net::OpCode::BeamState);
                                net::BeamStatePayload bp2{};
                                bp2.ownerNetId = net.id;
                                bp2.active = 1;
                                bp2.timeRemaining = weapon.beamActiveTime;
                                bp2.length = visualLength;
                                bp2.offsetY = 4.0f;
                                bpacket2 << bp2;
                                _networkSync.sendPacketToSessions(sessions, bpacket2, net::NetworkMode::TCP);
                            } else {
                                net::Packet bpacket(net::OpCode::BeamState);
                                net::BeamStatePayload bp{};
                                bp.ownerNetId = net.id;
                                bp.active = 1;
                                bp.timeRemaining = weapon.beamActiveTime;
                                bp.length = visualLength; // visual length in pixels (approx)
                                bp.offsetY = 0.0f;
                                bpacket << bp;
                                _networkSync.sendPacketToSessions(sessions, bpacket, net::NetworkMode::TCP);
                            }
                        }
                    }
                }
            }

            // Prevent normal shooting while beam active or in beam cooldown
            if (weapon.kind == ecs::components::WeaponKind::Beam && (weapon.beamActive || weapon.beamCooldownRemaining > 0.0f)) {
                // still update input state and ammo reloads, skip normal shot logic
                if (!shootPressed && shootWasPressed) input.chargeTime = 0.0f;
                input.lastMask = input.mask;
                if (ammo.dirty) {
                    sendAmmoUpdate(net.id, ammo);
                    ammo.dirty = false;
                }
                continue;
            }

            if (ammo.isReloading || (ammo.current == 0 && !hasInfiniteAmmo)) {
                input.chargeTime = 0.0f;
            }

            if (shootPressed && canShoot) {
                input.chargeTime = std::min(input.chargeTime + dt, kChargeMax);
            }

            const float fireInterval = (weapon.fireRate > 0.0f)
                ? (1.0f / weapon.fireRate)
                : 0.0f;

            if (!shootPressed && shootWasPressed) {
                if (canShoot) {
                    if (input.chargeTime >= kChargeMin && weapon.kind != ecs::components::WeaponKind::Beam) {
                        const float ratio = std::clamp(input.chargeTime / kChargeMax, 0.0f, 1.0f);
                        pendingChargedSpawns.emplace_back(entity, tf, roomId, ratio, hasDoubleFire);
                        if (!hasInfiniteAmmo && ammo.current > 0) {
                            ammo.current -= 1;
                        }
                        ammo.dirty = true;
                        weapon.lastShotTime = 0.0f;
                    } else if (weapon.lastShotTime >= fireInterval) {
                        pendingSpawns.emplace_back(entity, tf, roomId, hasDoubleFire);
                        if (!hasInfiniteAmmo && ammo.current > 0) {
                            ammo.current -= 1;
                        }
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

        for (const auto& [owner, tf, roomId, doubleFire] : pendingSpawns) {
            spawnBullet(owner, tf, roomId, doubleFire);
        }

        for (const auto& [owner, tf, roomId, ratio, doubleFire] : pendingChargedSpawns) {
            spawnChargedBullet(owner, tf, roomId, ratio, doubleFire);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void PlayerShootSystem::spawnBullet(
        ecs::Entity owner,
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

        // Use owner's weapon damage and size if available
        int damageAmount = 25;
        float boxW = 8.0f;
        float boxH = 4.0f;
        if (auto weaponRes = _registry.get<ecs::components::SimpleWeapon>()) {
            auto &weapons = weaponRes->get();
            if (weapons.has(owner)) {
                const auto &w = weapons[owner];
                damageAmount = w.damage;
                // Optionally adjust box size based on difficulty
                const float diffScale = 1.0f + (w.difficulty - 2) * 0.1f;
                boxW = 8.0f * diffScale;
                boxH = 4.0f * diffScale;
            }
        }

        _registry.add<ecs::components::BoundingBox>(
            bullet,
            ecs::components::BoundingBox{ boxW, boxH }
        );

        _registry.add<ecs::components::Damage>(
            bullet,
            ecs::components::Damage{ damageAmount, owner }
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

            // second bullet uses same damage/size as first
            _registry.add<ecs::components::BoundingBox>(
                bullet2,
                ecs::components::BoundingBox{ boxW, boxH }
            );

            _registry.add<ecs::components::Damage>(
                bullet2,
                ecs::components::Damage{ damageAmount, owner }
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
        ecs::Entity owner,
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
        const float baseW = 8.0f;
        const float baseH = 4.0f;
        const float sizeX = baseW * scale;
        const float sizeY = baseH * scale;
        const int minDamage = 25;
        const int maxDamage = 120;
        int damage = static_cast<int>(minDamage + (maxDamage - minDamage) * ratio);
        // Incorporate owner's weapon base damage if available
        if (auto weaponRes = _registry.get<ecs::components::SimpleWeapon>()) {
            auto &weapons = weaponRes->get();
            if (weapons.has(owner)) {
                const auto &w = weapons[owner];
                damage += w.damage; // add base damage
            }
        }

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
            ecs::components::Damage{ damage, owner }
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
                ecs::components::Damage{ damage, owner }
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
