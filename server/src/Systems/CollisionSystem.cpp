/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
**
 * CollisionSystem
*/

#include "Systems/CollisionSystem.hpp"

#include "RType/Logger.hpp"
#include "RType/Network/Packet.hpp"

#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <vector>

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

        if (!transformsRes ||
            !boxesRes ||
            !typesRes ||
            !roomsRes ||
            !healthRes ||
            !speedRes ||
            !powerupRes ||
            !damageRes) {
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
        
        // Optional components - only some entities have them
        auto velocitiesRes = _registry.get<ecs::components::Velocity>();
        auto *velocities = velocitiesRes ? &velocitiesRes->get() : nullptr;
        
        auto shieldsRes = _registry.get<ecs::components::Shield>();
        auto doubleFiresRes = _registry.get<ecs::components::DoubleFire>();

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

        auto updatePlayerScore = [&](uint32_t roomId, ecs::Entity playerEntity, int delta) {
            if (delta == 0 || playerEntity == ecs::NullEntity) {
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
                if (player->getEntityId() != static_cast<uint32_t>(playerEntity.index())) {
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

        auto sendPlayerHealth = [&](uint32_t roomId, ecs::Entity playerEntity, const ecs::components::Health &health) {
            auto room = _roomSystem.getRoom(roomId);
            if (!room) {
                return;
            }
            const auto playersInRoom = room->getPlayers();
            for (const auto &player : playersInRoom) {
                if (!player) {
                    continue;
                }
                if (player->getEntityId() != static_cast<uint32_t>(playerEntity.index())) {
                    continue;
                }
                net::Packet packet(net::OpCode::HealthUpdate);
                net::HealthUpdatePayload payload{health.currentHealth, health.maxHealth};
                packet << payload;
                _networkSync.sendPacketToSession(player->getId(), packet, net::NetworkMode::TCP);
                break;
            }
        };

        auto sendGameOverIfNeeded = [&](const std::shared_ptr<Room> &room) {
            if (!room) {
                return;
            }
            if (room->getState() != Room::State::InGame) {
                return;
            }
            if (room->hasActivePlayers()) {
                return;
            }

            room->forceFinishGame();

            const auto playersInRoom = room->getPlayers();
            if (playersInRoom.empty()) {
                return;
            }

            std::string bestName;
            int bestScore = 0;
            bool bestSet = false;

            for (const auto &player : playersInRoom) {
                if (!player) {
                    continue;
                }
                const int score = player->getScore();
                if (!bestSet || score > bestScore) {
                    bestSet = true;
                    bestScore = score;
                    bestName = player->getUsername();
                }
            }

            for (const auto &player : playersInRoom) {
                if (!player) {
                    continue;
                }
                net::Packet packet(net::OpCode::GameOver);
                net::GameOverPayload payload{};
                std::strncpy(payload.bestPlayer, bestName.c_str(), sizeof(payload.bestPlayer) - 1);
                payload.bestPlayer[sizeof(payload.bestPlayer) - 1] = '\0';
                payload.bestScore = bestScore;
                payload.playerScore = player->getScore();
                packet << payload;
                _networkSync.sendPacketToSession(player->getId(), packet, net::NetworkMode::TCP);
            }
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
                       type ==
                       net::EntityType::Boss ||
                       type ==
                       net::EntityType::BossShield) {
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
                       type == net::EntityType::PowerupSpeed ||
                       type == net::EntityType::PowerupDoubleFire ||
                       type == net::EntityType::PowerupShield) {
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
            auto *pvel = (velocities && velocities->has(player)) ? &(*velocities)[player] : nullptr;

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
                log::info("=== Power-up collision! Type: {} ===", static_cast<int>(powerup.type));
                
                if (powerup.type == ecs::components::PowerupType::Heal) {
                    // Red powerup - heal 1 heart (only if not at max health)
                    int oldHealth = health.currentHealth;
                    if (health.currentHealth < health.maxHealth) {
                        health.currentHealth = std::min(
                            health.maxHealth,
                            health.currentHealth + 1);
                        sendPlayerHealth(proom.id, player, health);
                        log::info("✚ HEAL: {} HP → {} HP (max: {})", oldHealth, health.currentHealth, health.maxHealth);
                    } else {
                        log::info("✚ HEAL: Already at max health ({}/{}), not applied", health.currentHealth, health.maxHealth);
                    }
                } else if (powerup.type == ecs::components::PowerupType::Speed) {
                    speed.multiplier =
                        std::max(speed.multiplier, powerup.value);
                    speed.boostRemaining =
                        std::max(speed.boostRemaining, powerup.duration);
                    log::info("⚡ SPEED: Multiplier={}, Duration={}s", speed.multiplier, speed.boostRemaining);
                } else if (powerup.type == ecs::components::PowerupType::DoubleFire) {
                    // Cyan powerup - double fire for 20 seconds
                    if (doubleFiresRes) {
                        auto& doubleFires = doubleFiresRes->get();
                        if (!doubleFires.has(player)) {
                            _registry.add<ecs::components::DoubleFire>(player, 20.0f);
                            log::info("🔫 DOUBLE FIRE: Added component (20s duration)");
                        } else {
                            doubleFires[player].remainingTime = 20.0f; // Reset timer
                            log::info("🔫 DOUBLE FIRE: Reset timer to 20s");
                        }
                    } else {
                        log::warning("DoubleFire component storage not available!");
                    }
                } else if (powerup.type == ecs::components::PowerupType::Shield) {
                    // Green powerup - shield that absorbs 1 hit
                    if (shieldsRes) {
                        auto& shields = shieldsRes->get();
                        if (!shields.has(player)) {
                            _registry.add<ecs::components::Shield>(player, 1);
                            log::info("🛡️  SHIELD: Added component (1 charge)");
                        } else {
                            shields[player].charges = 1; // Reset to 1 charge
                            log::info("🛡️  SHIELD: Reset to 1 charge");
                        }
                    } else {
                        log::warning("Shield component storage not available!");
                    }
                }

                log::info("Despawning power-up entity {}", powerEntity.index());
                markForDespawn(powerEntity, proom.id);
            }
        }

        for (auto player : players) {
            auto &ptf = transforms[player];
            auto &pbox = boxes[player];
            auto &proom = rooms[player];
            auto *pvel = (velocities && velocities->has(player)) ? &(*velocities)[player] : nullptr;

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

            auto boomerResLocal = _registry.get<ecs::components::Boomerang>();
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

                // Check if enemy is a Boss and if there are shields protecting it
                if (types[enemy].type == net::EntityType::Boss) {
                    // Count living BossShields in the same room
                    int shieldCount = 0;
                    for (auto potentialShield : enemies) {
                        if (rooms[potentialShield].id == broom.id &&
                            types[potentialShield].type == net::EntityType::BossShield &&
                            healths[potentialShield].currentHealth > 0) {
                            shieldCount++;
                        }
                    }
                    
                    // If shields exist, boss is protected - don't take damage
                    if (shieldCount > 0) {
                        bool isBoomer = (boomerResLocal && boomerResLocal->get().has(bullet));
                        if (!isBoomer) {
                            // Non-boomerang bullets despawn on shield
                            markForDespawn(bullet, broom.id);
                            break;
                        } else {
                            // Boomerang should not be destroyed by boss shields; skip damaging
                            continue;
                        }
                    }
                }

                health.currentHealth -= damage.amount;
                bool isBoomer = (boomerResLocal && boomerResLocal->get().has(bullet));
                if (!isBoomer) {
                    markForDespawn(bullet, broom.id);
                }
                if (health.currentHealth <= 0) {
                    const int award = getKillScore(types[enemy].type);
                    updatePlayerScore(broom.id, damage.sourceEntity, award);
                    // Enemy died - chance to drop power-up
                    const int dropChance = std::rand() % 100;
                    if (dropChance < 30) { // 30% chance to drop
                        spawnPowerup(etf.position, broom.id, dropChance);
                    }
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
                bool isBoomerObs = (boomerResLocal && boomerResLocal->get().has(bullet));
                if (!isBoomerObs) {
                    markForDespawn(bullet, broom.id);
                }
                if (types[obstacle].type ==
                    net::EntityType::Obstacle &&
                    health.currentHealth <= 0) {
                    markForDespawn(obstacle, broom.id);
                }
                break;
            }
        }

        // Handle boomerang returning to owner: recover ammo and despawn
        if (auto boomResCheck = _registry.get<ecs::components::Boomerang>()) {
            auto &boomers = boomResCheck->get();
            for (auto bullet : bullets) {
                if (removed.find(bullet.index()) != removed.end()) continue;
                if (!boomers.has(bullet)) continue;
                auto &b = boomers[bullet];
                if (!b.returning) continue;

                const auto &btf = transforms[bullet];
                const auto &bbox = boxes[bullet];

                // find owner entity in players list
                for (auto player : players) {
                    if (rooms[player].id != rooms[bullet].id) continue;
                    if (static_cast<uint32_t>(player.index()) != b.ownerIndex) continue;
                    const auto &ptf = transforms[player];
                    const auto &pbox = boxes[player];
                    if (!overlaps(ptf, pbox, btf, bbox)) continue;

                    // Recover ammo for owner
                    if (auto ammoRes = _registry.get<ecs::components::Ammo>()) {
                        auto &ammos = ammoRes->get();
                        if (ammos.has(player)) {
                            if (ammos[player].max > 0) {
                                ammos[player].current = static_cast<uint16_t>(std::min<int>(ammos[player].max, ammos[player].current + 1));
                            }
                            ammos[player].dirty = true;

                            // send ammo update packet to owner
                            net::Packet packet(net::OpCode::AmmoUpdate);
                            net::AmmoUpdatePayload payload{};
                            payload.current = ammos[player].current;
                            payload.max = ammos[player].max;
                            payload.isReloading = static_cast<uint8_t>(ammos[player].isReloading ? 1 : 0);
                            payload.cooldownRemaining = ammos[player].isReloading ? (ammos[player].reloadCooldown - ammos[player].reloadTimer) : 0.0f;
                            packet << payload;
                            if (auto netRes = _registry.get<ecs::components::NetworkId>()) {
                                auto &nets = netRes->get();
                                if (nets.has(player)) {
                                    _networkSync.sendPacketToSession(nets[player].id, packet, net::NetworkMode::TCP);
                                }
                            }
                        }
                    }

                    markForDespawn(bullet, rooms[bullet].id);
                    break;
                }
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

                // Check if player has shield
                bool shieldBlocked = false;
                if (shieldsRes) {
                    auto& shields = shieldsRes->get();
                    if (shields.has(player) && shields[player].charges > 0) {
                        // Shield absorbs the hit
                        shields[player].charges--;
                        if (shields[player].charges <= 0) {
                            _registry.remove<ecs::components::Shield>(player);
                            log::info("Player shield depleted");
                        } else {
                            log::info("Player shield blocked attack ({} charges left)", shields[player].charges);
                        }
                        shieldBlocked = true;
                    }
                }
                
                if (!shieldBlocked) {
                    // No shield - take damage
                    health.currentHealth =
                        std::max(0, health.currentHealth - damage.amount);
                    sendPlayerHealth(broom.id, player, health);
                }
                markForDespawn(bullet, broom.id);

                if (!shieldBlocked && health.currentHealth <= 0) {
                    auto room = _roomSystem.getRoom(broom.id);
                    if (room) {
                        const auto playersInRoom = room->getPlayers();
                        for (const auto &roomPlayer : playersInRoom) {
                            if (!roomPlayer) {
                                continue;
                            }
                            if (roomPlayer->getEntityId() == static_cast<uint32_t>(player.index())) {
                                room->setPlayerType(roomPlayer->getId(), Room::PlayerType::Spectator);
                                roomPlayer->setEntityId(0);
                                _networkSync.unbindSession(roomPlayer->getId());
                                break;
                            }
                        }
                        sendGameOverIfNeeded(room);
                    }
                    markForDespawn(player, broom.id);
                }
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
        log::debug("despawn() called for entity {}, roomId {}", entity.index(), roomId);
        auto transformRes = _registry.get<ecs::components::Transform>();
        auto typeRes = _registry.get<ecs::components::EntityType>();
        auto netRes = _registry.get<ecs::components::NetworkId>();
        if (!transformRes || !typeRes || !netRes) {
            log::debug("despawn() early return - missing component arrays");
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

        log::debug("Sending EntityDeath packet for netId={}, type={} to {} players",
                   net.id, static_cast<int>(type.type), players.size());

        for (const auto &player : players) {
            _networkSync.sendPacketToSession(player->getId(), packet,
                                             net::NetworkMode::TCP);
        }

        log::debug("Killing entity {} from registry", entity.index());
        _registry.kill(entity);
    }

    void CollisionSystem::spawnPowerup(const Vec2f& position, uint32_t roomId, int dropRoll)
    {
        auto entity = _registry.spawn();
        if (!entity) {
            log::error("Failed to spawn power-up entity");
            return;
        }

        // Determine power-up type based on drop roll (0-29 range for 30% drop)
        ecs::components::PowerupType type;
        net::EntityType netType;
        
        if (dropRoll < 10) {
            // 0-9: Red - Health (33% of drops)
            type = ecs::components::PowerupType::Heal;
            netType = net::EntityType::PowerupHeal;
        } else if (dropRoll < 20) {
            // 10-19: Yellow/White - Double Fire (33% of drops)
            type = ecs::components::PowerupType::DoubleFire;
            netType = net::EntityType::PowerupDoubleFire;
        } else {
            // 20-29: Green - Shield (33% of drops)
            type = ecs::components::PowerupType::Shield;
            netType = net::EntityType::PowerupShield;
        }

        auto e = entity.value();
        
        // Add components
        _registry.add<ecs::components::Transform>(e, position, 0.0f, Vec2f{1.0f, 1.0f});
        _registry.add<ecs::components::Velocity>(e, ecs::components::Velocity{Vec2f{-1.0f, 0.0f}, 30.0f}); // Move left slowly
        _registry.add<ecs::components::BoundingBox>(e, 16.0f, 16.0f);
        _registry.add<ecs::components::EntityType>(e, netType);
        _registry.add<ecs::components::RoomId>(e, roomId);
        _registry.add<ecs::components::Powerup>(e, type, 1.0f, 0.0f);
        
        // Assign network ID
        static uint32_t nextId = 1000;
        _registry.add<ecs::components::NetworkId>(e, nextId++);
        
        log::info("Spawned power-up type {} at ({}, {})", static_cast<int>(type), position.x, position.y);
        
        // Notify clients
        auto room = _roomSystem.getRoom(roomId);
        if (!room) {
            return;
        }

        const auto players = room->getPlayers();
        if (players.empty()) {
            return;
        }

        net::Packet packet(net::OpCode::EntitySpawn);
        net::EntitySpawnPayload payload{};
        payload.netId = nextId - 1;
        payload.type = static_cast<uint8_t>(netType);
        payload.posX = position.x;
        payload.posY = position.y;
        packet << payload;

        for (const auto &player : players) {
            _networkSync.sendPacketToSession(player->getId(), packet, net::NetworkMode::TCP);
        }
    }
}
