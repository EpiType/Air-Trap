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

using namespace rtp::net;

namespace rtp::client {
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    NetworkSyncSystem::NetworkSyncSystem(ClientNetwork& network, rtp::ecs::Registry& registry, EntityBuilder builder)
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
            rtp::net::PingPayload payload{ static_cast<uint64_t>(ms) };
            rtp::net::Packet packet(rtp::net::OpCode::Ping);
            packet << payload;
            _network.sendPacket(packet, rtp::net::NetworkMode::UDP);
        }
        while (auto eventOpt = _network.pollEvent()) {
            handleEvent(*eventOpt);
        }
    }

    void NetworkSyncSystem::tryLogin(const std::string& username, const std::string& password) const
    {
        rtp::net::LoginPayload payload;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);

        rtp::net::Packet packet(rtp::net::OpCode::LoginRequest);
        packet << payload;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryRegister(const std::string& username, const std::string& password) const
    {
        rtp::net::RegisterPayload payload;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);

        rtp::net::Packet packet(rtp::net::OpCode::RegisterRequest);
        packet << payload;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::requestListRooms(void)
    {
        rtp::log::info("Requesting list of available rooms from server...");
        if (_availableRooms.size() > 0)
            _availableRooms.clear();
        rtp::net::Packet packet(rtp::net::OpCode::ListRooms);
        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryCreateRoom(const std::string& roomName, uint32_t maxPlayers, float difficulty, float speed, uint32_t duration, uint32_t seed, uint32_t levelId)
    {
        rtp::log::info("Attempting to create room '{}' on server...", roomName);
        _currentState = State::CreatingRoom;
        rtp::net::CreateRoomPayload payload;
        std::strncpy(payload.roomName, roomName.c_str(), sizeof(payload.roomName) - 1);
        payload.maxPlayers = maxPlayers;
        payload.difficulty = difficulty;
        payload.speed = speed;
        payload.duration = duration;
        payload.seed = seed;
        payload.levelId = levelId;

        rtp::net::Packet packet(rtp::net::OpCode::CreateRoom);
        packet << payload;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryJoinRoom(uint32_t roomId, bool asSpectator)
    {
        _currentState = State::JoiningRoom;
        rtp::net::Packet packet(rtp::net::OpCode::JoinRoom);
        rtp::net::JoinRoomPayload payload{};
        payload.roomId = roomId;
        payload.isSpectator = static_cast<uint8_t>(asSpectator ? 1 : 0);
        packet << payload;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryLeaveRoom(void)
    {
        _currentState = State::LeavingRoom;
        rtp::net::Packet packet(rtp::net::OpCode::LeaveRoom);
        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
        trySetReady(false);
    }

    void NetworkSyncSystem::trySetReady(bool isReady)
    {
        rtp::net::Packet packet(rtp::net::OpCode::SetReady);
        packet << static_cast<uint8_t>(isReady ? 1 : 0);

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }
    
    void NetworkSyncSystem::trySendMessage(const std::string& message) const
    {
        rtp::net::Packet packet(rtp::net::OpCode::RoomChatSended);
        rtp::net::RoomChatPayload payload{};
        std::strncpy(payload.message, message.c_str(), sizeof(payload.message) - 1);
        payload.message[sizeof(payload.message) - 1] = '\0';
        packet << payload;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
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

    std::list<rtp::net::RoomInfo> NetworkSyncSystem::getAvailableRooms(void) const
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

    void NetworkSyncSystem::handleEvent(rtp::net::NetworkEvent& event)
    {
        switch (event.packet.header.opCode)
        {
            // case OpCode::EntitySpawn: { /* EntitySpawn */
            //     rtp::net::EntitySpawnPayload payload;
            //     event.packet >> payload;
            //     spawnEntityFromServer(payload);
            //     break;
            // }
            case OpCode::LoginResponse: {
                onLoginResponse(event.packet);
                break;
            }
            case OpCode::RegisterResponse: {
                onRegisterResponse(event.packet);
                break;
            }
            case OpCode::RoomList: {
                onRoomResponse(event.packet);
                break;
            }
            case OpCode::JoinRoom: {
                onJoinRoomResponse(event.packet);
                break;
            }
            case OpCode::CreateRoom: {
                onCreateRoomResponse(event.packet);
                break;
            }
            case OpCode::LeaveRoom: {
                onLeaveRoomResponse(event.packet);
                break;
            }
            case OpCode::StartGame: {
                rtp::log::info("Received StartGame notification from server.");
                _currentState = State::InGame;
                break;
            }
            case OpCode::EntitySpawn: {
                onSpawnEntityFromServer(event.packet);
                break;
            }
            case OpCode::EntityDeath: {
                onEntityDeath(event.packet);
                break;
            }
            case OpCode::RoomUpdate:
                onRoomUpdate(event.packet);
                break;
            case OpCode::RoomChatReceived: {
                onRoomChatReceived(event.packet);
                break;
            }
            case OpCode::AmmoUpdate: {
                onAmmoUpdate(event.packet);
                break;
            }
            case OpCode::Pong: {
                onPong(event.packet);
                break;
            }
            case OpCode::DebugModeUpdate: {
                onDebugModeUpdate(event.packet);
                break;
            }
            case OpCode::Kicked: {
                _kicked = true;
                _currentState = State::InLobby;
                _lastChatMessage.clear();
                _chatHistory.clear();
                break;
            }
            default:
                rtp::log::warning("Unhandled OpCode received: {}", static_cast<uint8_t>(event.packet.header.opCode));
                break;
        }
    }

    void NetworkSyncSystem::onLoginResponse(rtp::net::Packet& packet)
    {
        rtp::net::LoginResponsePayload payload;
        packet >> payload;

        if (payload.success) {
            rtp::log::info("Login successful for user '{}'", std::string(payload.username));
            _isLoggedIn = true;
            _username = std::string(payload.username);
            _currentState = State::InLobby;
        } else {
            rtp::log::warning("Login failed for user '{}'", std::string(payload.username));
            _isLoggedIn = false;
            _username = "";
            _currentState = State::NotLogged;
        }
    }

    void NetworkSyncSystem::onRegisterResponse(rtp::net::Packet& packet)
    {
        rtp::net::RegisterResponsePayload payload;
        packet >> payload;

        if (payload.success) {
            rtp::log::info("Registration successful for user '{}'", std::string(payload.username));
            _isLoggedIn = true;
            _username = std::string(payload.username);
            _currentState = State::InLobby;
        } else {
            rtp::log::warning("Registration failed for user '{}'", std::string(payload.username));
            _isLoggedIn = false;
            _username = "";
            _currentState = State::NotLogged;
        }
    }

    void NetworkSyncSystem::onRoomResponse(rtp::net::Packet& packet)
    {
        _availableRooms.clear();

        uint32_t roomCount = 0;
        packet >> roomCount;

        rtp::log::info("Received {} available rooms from server.", roomCount);

        if (roomCount <= 0) {
            rtp::log::info("No available rooms received from server.");
            return;
        }

        for (uint32_t i = 0; i < roomCount; ++i) {
            rtp::net::RoomInfo roomInfo{};
            packet >> roomInfo;
            _availableRooms.push_back(roomInfo);

            rtp::log::info(
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


    void NetworkSyncSystem::onJoinRoomResponse(rtp::net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;

        if (success) {
            rtp::log::info("Successfully joined the room.");
            _currentState = State::InRoom;
        } else {
            rtp::log::warning("Failed to join the room.");
        }
    }

    void NetworkSyncSystem::onCreateRoomResponse(rtp::net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;

        if (success) {
            rtp::log::info("Room created successfully.");
            _currentState = State::InRoom;
        } else {
            rtp::log::warning("Failed to create the room.");
        }
    }

    void NetworkSyncSystem::onLeaveRoomResponse(rtp::net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;

        if (success) {
            rtp::log::info("Successfully left the room.");
            _currentState = State::InLobby;
            _lastChatMessage.clear();
            _chatHistory.clear();
        } else {
            rtp::log::warning("Failed to leave the room.");
        }
    }

    void NetworkSyncSystem::onSpawnEntityFromServer(rtp::net::Packet& packet)
    {
        rtp::net::EntitySpawnPayload payload;
        packet >> payload;

        rtp::log::info("Spawning entity from server: NetID={}, Type={}, Position=({}, {})",
            payload.netId, static_cast<int>(payload.type), payload.posX, payload.posY);

        const rtp::Vec2f pos{payload.posX, payload.posY};

        EntityTemplate t;
        switch (static_cast<rtp::net::EntityType>(payload.type)) {
            case rtp::net::EntityType::Scout:
                t = EntityTemplate::rt2_4(pos);
                break;
            case rtp::net::EntityType::Tank:
                t = EntityTemplate::rt1_3(pos);
                break;
            case rtp::net::EntityType::Boss:
                t = EntityTemplate::rt1_13(pos);
                break;

            case rtp::net::EntityType::Player:
                t = EntityTemplate::rt1_1(pos);
                break;

            case rtp::net::EntityType::Bullet:
                t = EntityTemplate::createBulletEnemy(pos);
                break;
            case rtp::net::EntityType::EnemyBullet:
                t = EntityTemplate::createBulletEnemy(pos);
                break;

            case rtp::net::EntityType::PowerupHeal:
                t = EntityTemplate::rt1_4(pos);
                break;

            case rtp::net::EntityType::PowerupSpeed:
                t = EntityTemplate::rt1_5(pos);
                break;

            case rtp::net::EntityType::Obstacle:
                t = EntityTemplate::rt2_6(pos);
                break;
            case rtp::net::EntityType::ObstacleSolid:
                t = EntityTemplate::rt2_6(pos);
                break;

            default:
                rtp::log::warning("Unknown entity type {}, fallback template", (int)payload.type);
                t = EntityTemplate::rt2_2(pos);
                break;
        }

        auto res = _builder.spawn(t);
        if (!res) {
            rtp::log::error("Failed to spawn entity from template: {}", res.error().message());
            return;
        }

        auto e = res.value();

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            e, rtp::ecs::components::NetworkId{payload.netId}
        );

        const auto entityType = static_cast<rtp::net::EntityType>(payload.type);
        _registry.addComponent<rtp::ecs::components::EntityType>(
            e, rtp::ecs::components::EntityType{entityType}
        );

        if (payload.sizeX > 0.0f && payload.sizeY > 0.0f) {
            _registry.addComponent<rtp::ecs::components::BoundingBox>(
                e, rtp::ecs::components::BoundingBox{payload.sizeX, payload.sizeY}
            );
        } else if (entityType == rtp::net::EntityType::Obstacle ||
                   entityType == rtp::net::EntityType::ObstacleSolid) {
            _registry.addComponent<rtp::ecs::components::BoundingBox>(
                e, rtp::ecs::components::BoundingBox{32.0f, 32.0f}
            );
        }

        if (entityType == rtp::net::EntityType::Bullet) {
            if (auto transformsOpt = _registry.getComponents<rtp::ecs::components::Transform>(); transformsOpt) {
                auto &transforms = transformsOpt.value().get();
                if (transforms.has(e)) {
                    transforms[e].rotation = 180.0f;
                }
            }
        } else if (entityType == rtp::net::EntityType::EnemyBullet) {
            if (auto transformsOpt = _registry.getComponents<rtp::ecs::components::Transform>(); transformsOpt) {
                auto &transforms = transformsOpt.value().get();
                if (transforms.has(e)) {
                    transforms[e].rotation = 0.0f;
                }
            }
        }

        _netIdToEntity[payload.netId] = e;
    }

    void NetworkSyncSystem::onEntityDeath(rtp::net::Packet& packet)
    {
        rtp::net::EntityDeathPayload payload{};
        packet >> payload;

        auto it = _netIdToEntity.find(payload.netId);
        if (it == _netIdToEntity.end()) {
            return;
        }

        const rtp::ecs::Entity entity = it->second;
        _builder.kill(entity);
        _netIdToEntity.erase(it);
    }

    void NetworkSyncSystem::onRoomUpdate(rtp::net::Packet& packet)
    {
        rtp::net::RoomSnapshotPayload header{};
        std::vector<rtp::net::EntitySnapshotPayload> snapshots;

        packet >> header >> snapshots;

        for (const auto& snap : snapshots) {
            auto it = _netIdToEntity.find(snap.netId);
            if (it == _netIdToEntity.end()) {
                continue;
            }

            const rtp::ecs::Entity e = it->second;

            auto transformsOpt = _registry.getComponents<rtp::ecs::components::Transform>();
            if (!transformsOpt)
                continue;

            auto &transforms = transformsOpt.value().get();
            if (!transforms.has(e))
                continue;

            // rtp::log::debug("Updating entity NetID={} Position=({}, {}) Rotation={}",
            //     snap.netId, snap.position.x, snap.position.y, snap.rotation);
            transforms[e].position.x = snap.position.x;
            transforms[e].position.y = snap.position.y;
            transforms[e].rotation   = snap.rotation;
        }
    }

    void NetworkSyncSystem::onRoomChatReceived(rtp::net::Packet& packet)
    {
        rtp::net::RoomChatReceivedPayload payload{};
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

    void NetworkSyncSystem::onAmmoUpdate(rtp::net::Packet& packet)
    {
        rtp::net::AmmoUpdatePayload payload{};
        packet >> payload;

        _ammoCurrent = payload.current;
        _ammoMax = payload.max;
        _ammoReloading = payload.isReloading != 0;
        _ammoReloadRemaining = payload.cooldownRemaining;
    }

    void NetworkSyncSystem::onPong(rtp::net::Packet& packet)
    {
        rtp::net::PingPayload payload{};
        packet >> payload;
        const auto now = std::chrono::steady_clock::now();
        const auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
        if (ms >= static_cast<int64_t>(payload.clientTimeMs)) {
            _pingMs = static_cast<uint32_t>(ms - payload.clientTimeMs);
        }
    }

    void NetworkSyncSystem::onDebugModeUpdate(rtp::net::Packet& packet)
    {
        rtp::net::DebugModePayload payload{};
        packet >> payload;
        rtp::client::g_drawDebugBounds = (payload.enabled != 0);
    }
} // namespace rtp::client
