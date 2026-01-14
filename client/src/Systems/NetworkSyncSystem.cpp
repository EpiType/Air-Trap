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
#include "RType/Network/Packet.hpp"
#include "Game/EntityBuilder.hpp"
#include "Utils/DebugFlags.hpp"

#include <chrono>

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

    void NetworkSyncSystem::tryLogin(const std::string& username, const std::string& password) const
    {
        net::LoginPayload payload;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);

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

    void NetworkSyncSystem::tryCreateRoom(const std::string& roomName, uint32_t maxPlayers, float difficulty, float speed, uint32_t duration, uint32_t seed, uint32_t levelId)
    {
        log::info("Attempting to create room '{}' on server...", roomName);
        _currentState = State::CreatingRoom;
        net::CreateRoomPayload payload;
        std::strncpy(payload.roomName, roomName.c_str(), sizeof(payload.roomName) - 1);
        payload.maxPlayers = maxPlayers;
        payload.difficulty = difficulty;
        payload.speed = speed;
        payload.duration = duration;
        payload.seed = seed;
        payload.levelId = levelId;

        net::Packet packet(net::OpCode::CreateRoom);
        packet << payload;

        _network.sendPacket(packet, net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryJoinRoom(uint32_t roomId, bool asSpectator)
    {
        _currentState = State::JoiningRoom;
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
    
    void NetworkSyncSystem::trySendMessage(const std::string& message) const
    {
        net::Packet packet(net::OpCode::RoomChatSended);
        net::RoomChatPayload payload{};
        std::strncpy(payload.message, message.c_str(), sizeof(payload.message) - 1);
        payload.message[sizeof(payload.message) - 1] = '\0';
        packet << payload;

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

    std::string NetworkSyncSystem::getLastChatMessage(void) const
    {
        return _lastChatMessage;
    }

    const std::deque<std::string>& NetworkSyncSystem::getChatHistory(void) const
    {
        return _chatHistory;
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
            case net::OpCode::Pong: {
                onPong(event.packet);
                break;
            }
            case net::OpCode::DebugModeUpdate: {
                onDebugModeUpdate(event.packet);
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
        } else {
            log::warning("Failed to create the room.");
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
            case net::EntityType::Scout:
                t = EntityTemplate::rt2_4(pos);
                break;
            case net::EntityType::Tank:
                t = EntityTemplate::rt1_3(pos);
                break;
            case net::EntityType::Boss:
                t = EntityTemplate::rt1_13(pos);
                break;

            case net::EntityType::Player:
                t = EntityTemplate::rt1_1(pos);
                break;

            case net::EntityType::Bullet:
                t = EntityTemplate::createBulletEnemy(pos);
                break;
            case net::EntityType::ChargedBullet:
                t = EntityTemplate::createBulletEnemy(pos);
                if (payload.sizeX > 0.0f) {
                    float scale = 1.0f;
                    if (payload.sizeX <= 6.0f) {
                        scale = 0.5f;
                    } else if (payload.sizeX >= 12.0f) {
                        scale = 2.0f;
                    }
                    t.scale = {scale, scale};
                }
                break;
            case net::EntityType::EnemyBullet:
                t = EntityTemplate::createBulletEnemy(pos);
                break;

            case net::EntityType::PowerupHeal:
                t = EntityTemplate::rt1_4(pos);
                break;

            case net::EntityType::PowerupSpeed:
                t = EntityTemplate::rt1_5(pos);
                break;

            case net::EntityType::Obstacle:
                t = EntityTemplate::rt2_6(pos);
                break;
            case net::EntityType::ObstacleSolid:
                t = EntityTemplate::rt2_6(pos);
                break;

            default:
                log::warning("Unknown entity type {}, fallback template", (int)payload.type);
                t = EntityTemplate::rt2_2(pos);
                break;
        }

        auto res = _builder.spawn(t);
        if (!res) {
            log::error("Failed to spawn entity from template: {}", res.error().message());
            return;
        }

        auto e = res.value();

        _registry.add<ecs::components::NetworkId>(
            e, ecs::components::NetworkId{payload.netId}
        );

        const auto entityType = static_cast<net::EntityType>(payload.type);
        _registry.add<ecs::components::EntityType>(
            e, ecs::components::EntityType{entityType}
        );

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
        _builder.kill(entity);
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

            // log::debug("Updating entity NetID={} Position=({}, {}) Rotation={}",
            //     snap.netId, snap.position.x, snap.position.y, snap.rotation);
            transforms[e].position.x = snap.position.x;
            transforms[e].position.y = snap.position.y;
            transforms[e].rotation   = snap.rotation;
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

    void NetworkSyncSystem::onPong(net::Packet& packet)
    {
        net::PingPayload payload{};
        packet >> payload;
        const auto now = std::chrono::steady_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        if (ms >= static_cast<int64_t>(payload.clientTimeMs)) {
            _pingMs = static_cast<uint32_t>(ms - payload.clientTimeMs);
        }
    }

    void NetworkSyncSystem::onDebugModeUpdate(net::Packet& packet)
    {
        net::DebugModePayload payload{};
        packet >> payload;
        g_drawDebugBounds = (payload.enabled != 0);
    }
} // namespace rtp::client
