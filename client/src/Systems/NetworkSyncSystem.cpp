/**
 * File   : NetworkSyncSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "Systems/NetworkSyncSystem.hpp"
#include "RType/Logger.hpp"
#include "RType/ECS/Components/NetworkId.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Animation.hpp"
#include "RType/ECS/Components/EntityType.hpp"
#include "RType/ECS/Components/BoundingBox.hpp"
#include "RType/ECS/Components/ShieldVisual.hpp"
#include "RType/ECS/Components/Controllable.hpp"
#include "RType/ECS/Components/SimpleWeapon.hpp"
#include "RType/ECS/Components/Ammo.hpp"
#include "RType/Config/WeaponConfig.hpp"
#include "RType/Network/Packet.hpp"
#include "Game/EntityBuilder.hpp"
#include "Utils/DebugFlags.hpp"

#include <chrono>
#include <cmath>

namespace rtp::client {
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    NetworkSyncSystem::NetworkSyncSystem(ClientNetwork& network, ecs::Registry& registry, EntityBuilder builder)
        : _network(network), _registry(registry), _builder(builder) {}

    void NetworkSyncSystem::update(float dt)
    {
        if (_ammoReloading && _ammoReloadRemaining > 0.0f) {
            _ammoReloadRemaining -= dt;
            if (_ammoReloadRemaining < 0.0f)
                _ammoReloadRemaining = 0.0f;
        }
        _pingTimer += dt;
        if (_pingTimer >= _pingInterval) {
            _pingTimer = 0.0f;
            const auto now = std::chrono::steady_clock::now();
            const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
            net::PingPayload payload{ static_cast<uint64_t>(ms) };
            net::Packet packet(net::OpCode::Ping);
            packet << payload;
            _network.sendPacket(packet, net::NetworkMode::UDP);
        }
        while (auto eventOpt = _network.pollEvent()) {
            handleEvent(*eventOpt);
        }
    }

    void NetworkSyncSystem::tryLogin(const std::string& username, const std::string& password, uint8_t weaponKind) const
    {
        net::LoginPayload payload;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);
        payload.weaponKind = weaponKind;

        net::Packet packet(net::OpCode::LoginRequest);
        packet << payload;

        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryRegister(const std::string& username, const std::string& password) const
    {
        net::RegisterPayload payload;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);

        net::Packet packet(net::OpCode::RegisterRequest);
        packet << payload;

        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::requestListRooms(void)
    {
        log::info("Requesting list of available rooms from server...");
        if (_availableRooms.size() > 0)
            _availableRooms.clear();
        net::Packet packet(net::OpCode::ListRooms);
        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryCreateRoom(const std::string& roomName, uint32_t maxPlayers, float difficulty, float speed, uint32_t duration, uint32_t seed, uint32_t levelId, uint8_t roomType) {
        log::info("Attempting to create room '{}' on server...", roomName);
        _currentState = State::CreatingRoom;
        _currentLevelId = levelId;
        net::CreateRoomPayload payload;
        std::strncpy(payload.roomName, roomName.c_str(), sizeof(payload.roomName) - 1);
        payload.maxPlayers = maxPlayers;
        payload.difficulty = difficulty;
        payload.speed = speed;
        payload.duration = duration;
        payload.seed = seed;
        payload.levelId = levelId;
        payload.roomType = roomType;  // Set room type
        net::Packet packet(net::OpCode::CreateRoom);
        packet << payload;

        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryJoinRoom(uint32_t roomId, bool asSpectator)
    {
        _currentState = State::JoiningRoom;
        // Try to infer the level id from the latest room list
        for (const auto& r : _availableRooms) {
            if (r.roomId == roomId) {
                _currentLevelId = r.levelId;
                break;
            }
        }
        net::Packet packet(net::OpCode::JoinRoom);
        net::JoinRoomPayload payload{};
        payload.roomId = roomId;
        payload.isSpectator = static_cast<uint8_t>(asSpectator ? 1 : 0);
        packet << payload;

        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryLeaveRoom(void)
    {
        _currentState = State::LeavingRoom;
        net::Packet packet(net::OpCode::LeaveRoom);
        _network.sendPacket(packet, net::NetworkMode::TCP);
        trySetReady(false);
    }

    void NetworkSyncSystem::trySetReady(bool isReady)
    {
        net::Packet packet(net::OpCode::SetReady);
        packet << static_cast<uint8_t>(isReady ? 1 : 0);

        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryStartSolo(void) {
        _isStartingSolo = true;
        tryCreateRoom("Solo Game", 1, 0.5f, 1.0f, 10, 0, 1, static_cast<uint8_t>(net::roomType::Private));
    }
    
    void NetworkSyncSystem::trySendMessage(const std::string& message) const
    {
        net::Packet packet(net::OpCode::RoomChatSended);
        net::RoomChatPayload payload{};
        std::strncpy(payload.message, message.c_str(), sizeof(payload.message) - 1);
        payload.message[sizeof(payload.message) - 1] = '\0';
        packet << payload;

        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::sendSelectedWeapon(uint8_t weaponKind) const
    {
        log::info("Sending selected weapon {} to server", static_cast<int>(weaponKind));
        net::Packet packet(net::OpCode::UpdateSelectedWeapon);
        packet << weaponKind;
        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    //////////////////////////////////////////////////////////////////////////
    // Getters
    //////////////////////////////////////////////////////////////////////////

    bool NetworkSyncSystem::isInRoom(void) const
    {
        return _currentState == State::InRoom || _currentState == State::InGame;
    }

    bool NetworkSyncSystem::isReady(void) const
    {
        return _isReady;
    }

    bool NetworkSyncSystem::isUdpReady(void) const
    {
        return _network.isUdpReady();
    }

    bool NetworkSyncSystem::isLoggedIn(void) const
    {
        return _isLoggedIn;
    }

    std::string NetworkSyncSystem::getUsername(void) const
    {
        return _username;
    }

    std::list<net::RoomInfo> NetworkSyncSystem::getAvailableRooms(void) const
    {
        return _availableRooms;
    }

    bool NetworkSyncSystem::isInGame(void) const
    {
        return _currentState == State::InGame;
    }

    NetworkSyncSystem::State NetworkSyncSystem::getState(void) const
    {
        return _currentState;
    }

    uint16_t NetworkSyncSystem::getAmmoCurrent(void) const
    {
        return _ammoCurrent;
    }

    uint16_t NetworkSyncSystem::getAmmoMax(void) const
    {
        return _ammoMax;
    }

    bool NetworkSyncSystem::isReloading(void) const
    {
        return _ammoReloading;
    }

    float NetworkSyncSystem::getReloadCooldownRemaining(void) const
    {
        return _ammoReloadRemaining;
    }

    uint32_t NetworkSyncSystem::getPingMs(void) const
    {
        return _pingMs;
    }

    bool NetworkSyncSystem::consumeKicked(void)
    {
        const bool kicked = _kicked;
        _kicked = false;
        return kicked;
    }

    uint32_t NetworkSyncSystem::getCurrentLevelId(void) const
    {
        return _currentLevelId;
    }

    std::string NetworkSyncSystem::getLastChatMessage(void) const
    {
        return _lastChatMessage;
    }

    const std::deque<std::string>& NetworkSyncSystem::getChatHistory(void) const
    {
        return _chatHistory;
    }

    void NetworkSyncSystem::setGameOverSummary(const GameOverSummary& summary)
    {
        _gameOverSummary = summary;
    }

    NetworkSyncSystem::GameOverSummary NetworkSyncSystem::getGameOverSummary(void) const
    {
        return _gameOverSummary;
    }

    bool NetworkSyncSystem::consumeGameOver(void)
    {
        if (!_gameOverPending) {
            return false;
        }
        _gameOverPending = false;
        return true;
    }

    int NetworkSyncSystem::getScore(void) const
    {
        return _score;
    }

    int NetworkSyncSystem::getHealthCurrent(void) const
    {
        return _healthCurrent;
    }

    int NetworkSyncSystem::getHealthMax(void) const
    {
        return _healthMax;
    }

    //////////////////////////////////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////////////////////////////////

    void NetworkSyncSystem::handleEvent(net::NetworkEvent& event)
    {
        switch (event.packet.header.opCode)
        {
            // case net::OpCode::EntitySpawn: { /* EntitySpawn */
            //     net::EntitySpawnPayload payload;
            //     event.packet >> payload;
            //     spawnEntityFromServer(payload);
            //     break;
            // }
            case net::OpCode::LoginResponse: {
                onLoginResponse(event.packet);
                break;
            }
            case net::OpCode::RegisterResponse: {
                onRegisterResponse(event.packet);
                break;
            }
            case net::OpCode::RoomList: {
                onRoomResponse(event.packet);
                break;
            }
            case net::OpCode::JoinRoom: {
                onJoinRoomResponse(event.packet);
                break;
            }
            case net::OpCode::CreateRoom: {
                onCreateRoomResponse(event.packet);
                break;
            }
            case net::OpCode::LeaveRoom: {
                onLeaveRoomResponse(event.packet);
                break;
            }
            case net::OpCode::StartGame: {
                log::info("Received StartGame notification from server.");
                _currentState = State::InGame;
                break;
            }
            case net::OpCode::EntitySpawn: {
                onSpawnEntityFromServer(event.packet);
                break;
            }
            case net::OpCode::EntityDeath: {
                onEntityDeath(event.packet);
                break;
            }
            case net::OpCode::RoomUpdate:
                onRoomUpdate(event.packet);
                break;
            case net::OpCode::RoomChatReceived: {
                onRoomChatReceived(event.packet);
                break;
            }
            case net::OpCode::AmmoUpdate: {
                onAmmoUpdate(event.packet);
                break;
            }
            case net::OpCode::BeamState: {
                onBeamState(event.packet);
                break;
            }
            case net::OpCode::Pong: {
                onPong(event.packet);
                break;
            }
            case net::OpCode::DebugModeUpdate: {
                onDebugModeUpdate(event.packet);
                break;
            }
            case net::OpCode::ScoreUpdate: {
                net::ScoreUpdatePayload payload{};
                event.packet >> payload;
                _score = payload.score;
                break;
            }
            case net::OpCode::HealthUpdate: {
                net::HealthUpdatePayload payload{};
                event.packet >> payload;
                _healthCurrent = payload.current;
                _healthMax = payload.max;
                break;
            }
            case net::OpCode::GameOver: {
                net::GameOverPayload payload{};
                event.packet >> payload;
                GameOverSummary summary;
                summary.bestPlayer = std::string(payload.bestPlayer);
                summary.bestScore = payload.bestScore;
                summary.playerScore = payload.playerScore;
                summary.isWin = payload.isWin;
                _gameOverSummary = summary;
                _gameOverPending = true;
                break;
            }
            case net::OpCode::Kicked: {
                _kicked = true;
                _currentState = State::InLobby;
                _lastChatMessage.clear();
                _chatHistory.clear();
                break;
            }
            default:
                log::warning("Unhandled OpCode received: {}", static_cast<uint8_t>(event.packet.header.opCode));
                break;
        }
    }

    void NetworkSyncSystem::onLoginResponse(net::Packet& packet)
    {
        net::LoginResponsePayload payload;
        packet >> payload;

        if (payload.success) {
            log::info("Login successful for user '{}'", std::string(payload.username));
            _isLoggedIn = true;
            _username = std::string(payload.username);
            _currentState = State::InLobby;
        } else {
            log::warning("Login failed for user '{}'", std::string(payload.username));
            _isLoggedIn = false;
            _username = "";
            _currentState = State::NotLogged;
        }
    }

    void NetworkSyncSystem::onRegisterResponse(net::Packet& packet)
    {
        net::RegisterResponsePayload payload;
        packet >> payload;

        if (payload.success) {
            log::info("Registration successful for user '{}'", std::string(payload.username));
            _isLoggedIn = true;
            _username = std::string(payload.username);
            _currentState = State::InLobby;
        } else {
            log::warning("Registration failed for user '{}'", std::string(payload.username));
            _isLoggedIn = false;
            _username = "";
            _currentState = State::NotLogged;
        }
    }

    void NetworkSyncSystem::onRoomResponse(net::Packet& packet)
    {
        _availableRooms.clear();

        uint32_t roomCount = 0;
        packet >> roomCount;

        log::info("Received {} available rooms from server.", roomCount);

        if (roomCount <= 0) {
            log::info("No available rooms received from server.");
            return;
        }

        for (uint32_t i = 0; i < roomCount; ++i) {
            net::RoomInfo roomInfo{};
            packet >> roomInfo;
            _availableRooms.push_back(roomInfo);

            log::info(
                "Received Room: ID={} Name='{}' Players={}/{} InGame={} Difficulty={} Speed={} Duration={} Seed={} LevelID={}",
                roomInfo.roomId,
                std::string(roomInfo.roomName),
                roomInfo.currentPlayers,
                roomInfo.maxPlayers,
                (int)roomInfo.inGame,
                roomInfo.difficulty,
                roomInfo.speed,
                roomInfo.duration,
                roomInfo.seed,
                roomInfo.levelId
            );
        }
    }


    void NetworkSyncSystem::onJoinRoomResponse(net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;

        if (success) {
            log::info("Successfully joined the room.");
            _currentState = State::InRoom;
        } else {
            log::warning("Failed to join the room.");
        }
    }

    void NetworkSyncSystem::onCreateRoomResponse(net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;
        if (success) {
            log::info("Room created successfully.");
            _currentState = State::InRoom;
            if (_isStartingSolo) {
                trySetReady(true);
                _isStartingSolo = false;
            }
        } else {
            log::warning("Failed to create the room.");
            _isStartingSolo = false;
        }
    }

    void NetworkSyncSystem::onLeaveRoomResponse(net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;

        if (success) {
            log::info("Successfully left the room.");
            _currentState = State::InLobby;
            _lastChatMessage.clear();
            _chatHistory.clear();
        } else {
            log::warning("Failed to leave the room.");
        }
    }

    void NetworkSyncSystem::onSpawnEntityFromServer(net::Packet& packet)
    {
        net::EntitySpawnPayload payload;
        packet >> payload;

        log::info("Spawning entity from server: NetID={}, Type={}, Position=({}, {})",
            payload.netId, static_cast<int>(payload.type), payload.posX, payload.posY);

        const Vec2f pos{payload.posX, payload.posY};

        EntityTemplate t;
        switch (static_cast<net::EntityType>(payload.type)) {
            case net::EntityType::Boss3Invincible:
                t = EntityTemplate::createBoss3Invincible(pos);
                break;
                        case net::EntityType::Enemy4:
                            t = EntityTemplate::enemy_4(pos);
                            break;
            case net::EntityType::Enemy1:
                t = EntityTemplate::enemy_1(pos);
                break;
            case net::EntityType::Enemy2:
                t = EntityTemplate::enemy_2(pos);
                break;
            case net::EntityType::Enemy3:
                t = EntityTemplate::enemy_3(pos);
                break;
            case net::EntityType::Tank:
                t = EntityTemplate::effect_1(pos);
                break;
            case net::EntityType::Boss:
                t = EntityTemplate::createBossShip(pos);
                break;
            case net::EntityType::Boss2:
                t = EntityTemplate::createBoss2Kraken(pos);
                break;
            case net::EntityType::Boss2Bullet:
                t = EntityTemplate::createBoss2Bullet(pos);
                break;

            case net::EntityType::Player:
                t = EntityTemplate::player_ship(pos);
                break;

            case net::EntityType::Bullet:
                t = EntityTemplate::createBulletPlayer(pos);
                if (payload.weaponKind == static_cast<uint8_t>(ecs::components::WeaponKind::Boomerang)) {
                    t = EntityTemplate::shot_1(pos);
                    t.tag = "boomerang_bullet";
                }
                break;
            case net::EntityType::ChargedBullet:
                t = EntityTemplate::createBulletPlayer(pos);
                if (payload.weaponKind == static_cast<uint8_t>(ecs::components::WeaponKind::Boomerang)) {
                    t = EntityTemplate::shot_1(pos);
                    t.tag = "boomerang_bullet";
                }
                if (payload.sizeX > 0.0f) {
                    float scale = 1.0f;
                    if (payload.sizeX <= 6.0f) {
                        scale = 0.5f;
                    } else if (payload.sizeX < 12.0f) {
                        scale = 1.0f;
                    } else if (payload.sizeX < 16.0f) {
                        scale = 1.5f;
                    } else {
                        scale = 2.0f;
                    }
                    t.scale = {scale, scale};
                }
                break;
            case net::EntityType::EnemyBullet:
                t = EntityTemplate::createBulletEnemy(pos);
                break;

            case net::EntityType::Obstacle:
                t = EntityTemplate::effect_4(pos);
                break;
            case net::EntityType::ObstacleSolid:
                t = EntityTemplate::effect_4(pos);
                break;
            case net::EntityType::PowerupHeal:
                log::debug("Creating PowerupHeal template at ({}, {})", pos.x, pos.y);
                t = EntityTemplate::createPowerUpHeal(pos);
                break;
            case net::EntityType::PowerupSpeed:
                t = EntityTemplate::createPowerUpHeal(pos);
                break;
            case net::EntityType::PowerupDoubleFire:
                log::debug("Creating PowerupDoubleFire template at ({}, {})", pos.x, pos.y);
                t = EntityTemplate::createPowerUpDoubleFire(pos);
                break;
            case net::EntityType::PowerupShield:
                log::debug("Creating PowerupShield template at ({}, {})", pos.x, pos.y);
                t = EntityTemplate::createPowerUpShield(pos);
                break;
            case net::EntityType::BossShield:
                log::debug("Creating BossShield template at ({}, {})", pos.x, pos.y);
                t = EntityTemplate::createBossShield(pos);
                break;

            default:
                log::warning("Unknown entity type {}, fallback template", (int)payload.type);
                t = EntityTemplate::enemy_1(pos);
                break;
        }

        log::debug("About to spawn entity template, tag='{}'", t.tag);
        auto res = _builder.spawn(t);
        if (!res) {
            log::error("Failed to spawn entity from template: {}", res.error().message());
            return;
        }
        log::debug("Successfully spawned entity from template");

        auto e = res.value();

        _registry.add<ecs::components::NetworkId>(
            e, ecs::components::NetworkId{payload.netId}
        );

        const auto entityType = static_cast<net::EntityType>(payload.type);
        _registry.add<ecs::components::EntityType>(
            e, ecs::components::EntityType{entityType}
        );

        if (entityType == net::EntityType::Player) {
            ecs::components::SimpleWeapon wcfg;
            wcfg.kind = static_cast<ecs::components::WeaponKind>(payload.weaponKind);

            if (rtp::config::hasWeaponConfigs()) {
                wcfg = rtp::config::getWeaponDef(wcfg.kind);
                wcfg.kind = static_cast<ecs::components::WeaponKind>(payload.weaponKind);
            } else {
                switch (wcfg.kind) {
                    case ecs::components::WeaponKind::Classic:
                        wcfg.fireRate = 6.0f; wcfg.damage = 10; wcfg.ammo = -1; wcfg.maxAmmo = -1; break;
                    case ecs::components::WeaponKind::Beam:
                        wcfg.fireRate = 0.0f;
                        wcfg.damage = 4;
                        wcfg.beamDuration = 5.0f;
                        wcfg.beamCooldown = 3.0f;
                        wcfg.maxAmmo = 1;
                        break;
                    case ecs::components::WeaponKind::Tracker:
                        wcfg.fireRate = 2.0f; wcfg.damage = 6; wcfg.homing = true; wcfg.ammo = 50; wcfg.maxAmmo = 50; break;
                    case ecs::components::WeaponKind::Boomerang:
                        wcfg.fireRate = 0.5f; wcfg.damage = 18; wcfg.isBoomerang = true; wcfg.ammo = -1; wcfg.maxAmmo = -1; break;
                    default:
                        break;
                }
            }

            if (auto weaponsOpt = _registry.get<ecs::components::SimpleWeapon>()) {
                auto &weapons = weaponsOpt.value().get();
                if (weapons.has(e)) {
                    weapons[e] = wcfg;
                } else {
                    _registry.add<ecs::components::SimpleWeapon>(e, wcfg);
                }
            } else {
                _registry.add<ecs::components::SimpleWeapon>(e, wcfg);
            }

            if (auto ammoOpt = _registry.get<ecs::components::Ammo>()) {
                auto &ammos = ammoOpt.value().get();
                if (ammos.has(e)) {
                    if (wcfg.maxAmmo >= 0) {
                        ammos[e].max = static_cast<uint16_t>(wcfg.maxAmmo);
                        if (wcfg.ammo > 0)
                            ammos[e].current = static_cast<uint16_t>(wcfg.ammo);
                    }
                } else {
                    uint16_t max = (wcfg.maxAmmo >= 0) ? static_cast<uint16_t>(wcfg.maxAmmo) : 100;
                    uint16_t cur = (wcfg.ammo >= 0) ? static_cast<uint16_t>( (wcfg.ammo > 0) ? wcfg.ammo : max ) : max;
                    _registry.add<ecs::components::Ammo>(e, ecs::components::Ammo{cur, max, 2.0f, 0.0f, false, true});
                }
            } else {
                uint16_t max = (wcfg.maxAmmo >= 0) ? static_cast<uint16_t>(wcfg.maxAmmo) : 100;
                uint16_t cur = (wcfg.ammo >= 0) ? static_cast<uint16_t>( (wcfg.ammo > 0) ? wcfg.ammo : max ) : max;
                _registry.add<ecs::components::Ammo>(e, ecs::components::Ammo{cur, max, 2.0f, 0.0f, false, true});
            }

        }

        if (payload.sizeX > 0.0f && payload.sizeY > 0.0f) {
            _registry.add<ecs::components::BoundingBox>(
                e, ecs::components::BoundingBox{payload.sizeX, payload.sizeY}
            );
        } else if (entityType == net::EntityType::Obstacle ||
                   entityType == net::EntityType::ObstacleSolid) {
            _registry.add<ecs::components::BoundingBox>(
                e, ecs::components::BoundingBox{32.0f, 32.0f}
            );
        }

        if (entityType == net::EntityType::Bullet ||
            entityType == net::EntityType::ChargedBullet) {
            if (auto transformsOpt = _registry.get<ecs::components::Transform>(); transformsOpt) {
                auto &transforms = transformsOpt.value().get();
                if (transforms.has(e)) {
                    transforms[e].rotation = 180.0f;
                }
            }
        } else if (entityType == net::EntityType::EnemyBullet) {
            if (auto transformsOpt = _registry.get<ecs::components::Transform>(); transformsOpt) {
                auto &transforms = transformsOpt.value().get();
                if (transforms.has(e)) {
                    transforms[e].rotation = 0.0f;
                }
            }
        }

        _netIdToEntity[payload.netId] = e;
    }

    void NetworkSyncSystem::onEntityDeath(net::Packet& packet)
    {
        net::EntityDeathPayload payload{};
        packet >> payload;

        auto it = _netIdToEntity.find(payload.netId);
        if (it == _netIdToEntity.end()) {
            return;
        }

        const ecs::Entity entity = it->second;
        
        // Vérifier si c'est un power-up Shield qui a été collecté
        auto entityTypesOpt = _registry.get<ecs::components::EntityType>();
        if (entityTypesOpt) {
            auto& entityTypes = entityTypesOpt.value().get();
            if (entityTypes.has(entity)) {
                const auto entityType = entityTypes[entity].type;
                log::debug("EntityDeath: netId={}, entityType={}", payload.netId, static_cast<int>(entityType));
                
                if (entityType == net::EntityType::PowerupShield) {
                    log::info("🛡️ PowerupShield collected!");
                    auto playerTypesOpt = _registry.get<ecs::components::EntityType>();
                    auto shieldVisualsOpt = _registry.get<rtp::ecs::components::ShieldVisual>();
                    
                    if (playerTypesOpt && shieldVisualsOpt) {
                        auto& playerTypes = playerTypesOpt.value().get();
                        auto& shieldVisuals = shieldVisualsOpt.value().get();
                        
                        int playerCount = 0;
                        for (auto playerEntity : playerTypes.entities()) {
                            if (!playerTypes.has(playerEntity)) continue;
                            if (playerTypes[playerEntity].type != net::EntityType::Player) continue;
                            
                            playerCount++;
                            
                            if (!shieldVisuals.has(playerEntity)) {
                                _registry.add<rtp::ecs::components::ShieldVisual>(playerEntity, rtp::ecs::components::ShieldVisual{});
                            } else {
                                shieldVisuals[playerEntity].animationTime = 0.0f;
                                shieldVisuals[playerEntity].alpha = 255.0f;
                            }
                        }
                    } else {
                        log::warning("Could not get playerTypes or shieldVisuals!");
                    }
                }
                else if (entityType == net::EntityType::EnemyBullet) {
                    auto playerTypesOpt = _registry.get<ecs::components::EntityType>();
                    auto shieldVisualsOpt = _registry.get<rtp::ecs::components::ShieldVisual>();
                    auto transformsOpt = _registry.get<ecs::components::Transform>();
                    
                    if (playerTypesOpt && shieldVisualsOpt && transformsOpt) {
                        auto& playerTypes = playerTypesOpt.value().get();
                        auto& shieldVisuals = shieldVisualsOpt.value().get();
                        auto& transforms = transformsOpt.value().get();
                        
                        for (auto playerEntity : shieldVisuals.entities()) {
                            if (!shieldVisuals.has(playerEntity)) continue;
                            if (!playerTypes.has(playerEntity)) continue;
                            if (playerTypes[playerEntity].type != net::EntityType::Player) continue;
                            
                            if (transforms.has(playerEntity) && transforms.has(entity)) {
                                const auto& playerPos = transforms[playerEntity].position;
                                const auto& bulletPos = transforms[entity].position;
                                
                                const float dx = bulletPos.x - playerPos.x;
                                const float dy = bulletPos.y - playerPos.y;
                                const float distance = std::sqrt(dx * dx + dy * dy);
                                
                                if (distance < 150.0f) {
                                    _registry.remove<rtp::ecs::components::ShieldVisual>(playerEntity);
                                    log::info("🛡️ Shield absorbed bullet!");
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        
        _registry.kill(entity);
        _netIdToEntity.erase(it);
    }

    void NetworkSyncSystem::onRoomUpdate(net::Packet& packet)
    {
        net::RoomSnapshotPayload header{};
        std::vector<net::EntitySnapshotPayload> snapshots;

        packet >> header >> snapshots;

        for (const auto& snap : snapshots) {
            auto it = _netIdToEntity.find(snap.netId);
            if (it == _netIdToEntity.end()) {
                continue;
            }

            const ecs::Entity e = it->second;

            auto transformsOpt = _registry.get<ecs::components::Transform>();
            if (!transformsOpt)
                continue;

            auto &transforms = transformsOpt.value().get();
            if (!transforms.has(e))
                continue;

            transforms[e].position.x = snap.position.x;
            transforms[e].position.y = snap.position.y;
            transforms[e].rotation   = snap.rotation;
            auto beamIt = _beamEntities.find(snap.netId);
            if (beamIt != _beamEntities.end()) {
                auto &vec = beamIt->second; // vector<pair<Entity, offsetY>>
                if (auto tOpt2 = _registry.get<ecs::components::Transform>(); tOpt2) {
                    auto &transforms2 = tOpt2.value().get();
                    const float spriteW = 81.0f;

                    // Compute frontOffset from player's sprite width
                    float frontOffset = 20.0f;
                    ecs::Entity ownerEntity = e;
                    if (auto sOpt = _registry.get<ecs::components::Sprite>(); sOpt) {
                        auto &sprites = sOpt.value().get();
                        if (sprites.has(ownerEntity)) {
                            const float playerSpriteW = static_cast<float>(sprites[ownerEntity].rectWidth);
                            if (auto t3Opt = _registry.get<ecs::components::Transform>(); t3Opt) {
                                auto &transforms3 = t3Opt.value().get();
                                if (transforms3.has(ownerEntity)) {
                                    const float playerScaleX = std::abs(transforms3[ownerEntity].scale.x);
                                    frontOffset = (playerSpriteW * playerScaleX) * 0.5f + 4.0f;
                                }
                            }
                        }
                    }

                    auto lenIt = _beamLengths.find(snap.netId);
                    for (size_t i = 0; i < vec.size(); ++i) {
                        ecs::Entity beamEntity = vec[i].first;
                        float offsetY = vec[i].second;
                        float length = 0.0f;
                        if (lenIt != _beamLengths.end() && i < lenIt->second.size()) length = lenIt->second[i];

                        if (!transforms2.has(beamEntity)) continue;
                            float scaleMag = (length > 0.0f) ? (length / spriteW) : std::abs(transforms2[beamEntity].scale.x);
                            transforms2[beamEntity].rotation = 0.0f;
                            transforms2[beamEntity].scale.x = -std::abs(scaleMag);
                            const float scaledWidth = spriteW * std::abs(transforms2[beamEntity].scale.x);
                        transforms2[beamEntity].position.x = snap.position.x + frontOffset + (scaledWidth * 0.5f);
                        transforms2[beamEntity].position.y = snap.position.y + offsetY;
                    }
                }
            }
        }
    }

    void NetworkSyncSystem::onRoomChatReceived(net::Packet& packet)
    {
        net::RoomChatReceivedPayload payload{};
        packet >> payload;

        std::string username(payload.username);
        std::string message(payload.message);
        const std::string line = username.empty()
            ? message
            : (username + ": " + message);

        pushChatMessage(line);
    }

    void NetworkSyncSystem::pushChatMessage(const std::string& message)
    {
        _lastChatMessage = message;
        if (message.empty())
            return;
        _chatHistory.push_back(message);
        while (_chatHistory.size() > _chatHistoryLimit) {
            _chatHistory.pop_front();
        }
    }

    void NetworkSyncSystem::onAmmoUpdate(net::Packet& packet)
    {
        net::AmmoUpdatePayload payload{};
        packet >> payload;

        _ammoCurrent = payload.current;
        _ammoMax = payload.max;
        _ammoReloading = payload.isReloading != 0;
        _ammoReloadRemaining = payload.cooldownRemaining;
    }

    void NetworkSyncSystem::onBeamState(net::Packet& packet)
    {
        net::BeamStatePayload payload{};
        packet >> payload;

        auto it = _netIdToEntity.find(payload.ownerNetId);
        if (it == _netIdToEntity.end()) {
            return;
        }

    

        const ecs::Entity ownerEntity = it->second;

        if (payload.active) {
            Vec2f pos{0.0f, 0.0f};
            if (auto tOpt = _registry.get<ecs::components::Transform>(); tOpt) {
                auto &transforms = tOpt.value().get();
                if (transforms.has(ownerEntity)) {
                    pos = transforms[ownerEntity].position;
                }
            }

            EntityTemplate t = EntityTemplate::shot_5(pos);
            const float spriteW = 81.0f;

            float frontOffset = 20.0f;
            if (auto sOpt = _registry.get<ecs::components::Sprite>(); sOpt) {
                auto &sprites = sOpt.value().get();
                if (sprites.has(ownerEntity)) {
                    const float playerSpriteW = static_cast<float>(sprites[ownerEntity].rectWidth);
                    if (auto t3Opt = _registry.get<ecs::components::Transform>(); t3Opt) {
                        auto &transforms3 = t3Opt.value().get();
                        if (transforms3.has(ownerEntity)) {
                            const float playerScaleX = std::abs(transforms3[ownerEntity].scale.x);
                            frontOffset = (playerSpriteW * playerScaleX) * 0.5f + 4.0f;
                        }
                    }
                }
            }

            float scaleMag = (payload.length > 0.0f) ? (payload.length / spriteW) : 1.0f;
            t.rotation = 0.0f;
            t.scale.x = -std::abs(scaleMag);
            t.scale.y = 1.0f;
            t.tag = "beam_visual";
            const float scaledWidth = spriteW * std::abs(t.scale.x);
            t.position.x = pos.x + frontOffset + (scaledWidth * 0.5f);
            t.position.y = pos.y + payload.offsetY;

            auto res = _builder.spawn(t);
            if (!res) {
                log::error("Failed to spawn beam visual: {}", res.error().message());
                return;
            }
            auto beamEntity = res.value();
            _beamEntities[payload.ownerNetId].push_back({beamEntity, payload.offsetY});
            _beamLengths[payload.ownerNetId].push_back(payload.length);
        } else {
            auto it2 = _beamEntities.find(payload.ownerNetId);
            if (it2 != _beamEntities.end()) {
                auto &vec = it2->second;
                for (size_t i = 0; i < vec.size(); ++i) {
                    if (std::fabs(vec[i].second - payload.offsetY) < 0.1f) {
                        ecs::Entity beamEntity = vec[i].first;
                        _builder.kill(beamEntity);
                        vec.erase(vec.begin() + i);
                        auto lenIt = _beamLengths.find(payload.ownerNetId);
                        if (lenIt != _beamLengths.end() && i < lenIt->second.size()) lenIt->second.erase(lenIt->second.begin() + i);
                        break;
                    }
                }
                if (vec.empty()) {
                    _beamEntities.erase(it2);
                    _beamLengths.erase(payload.ownerNetId);
                }
            }
        }
        
    }

    void NetworkSyncSystem::onPong(net::Packet& packet)
    {
        net::PingPayload payload{};
        packet >> payload;
        const auto now = std::chrono::steady_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        if (static_cast<uint64_t>(ms) >= payload.clientTimeMs) {
            _pingMs = static_cast<uint32_t>(static_cast<uint64_t>(ms) - payload.clientTimeMs);
        } else {
            _pingMs = 0;
        }
    }

    void NetworkSyncSystem::onDebugModeUpdate(net::Packet& packet)
    {
        net::DebugModePayload payload{};
        packet >> payload;
        g_drawDebugBounds = (payload.enabled != 0);
    }
} // namespace rtp::client
