/**
 * File   : NetworkSyncSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/NetworkSyncSystem.hpp"

#include "engine/core/Logger.hpp"

#include <chrono>
#include <cstring>

namespace rtp::client
{
    NetworkSyncSystem::NetworkSyncSystem(engine::net::INetwork& network,
                                         engine::ecs::Registry& registry,
                                         EntityBuilder builder)
        : _network(network),
          _registry(registry),
          _builder(std::move(builder))
    {
    }

    /////////////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////////////

    void NetworkSyncSystem::update(float dt)
    {
        while (auto event = _network.pollEvent()) {
            handleEvent(*event);
        }

        _pingTimer += dt;
        if (_pingTimer >= _pingInterval) {
            _pingTimer = 0.0f;
            auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now());
            const uint64_t ms =
                static_cast<uint64_t>(now.time_since_epoch().count());

            rtp::net::PingPayload payload{ms};
            rtp::net::Packet packet(rtp::net::OpCode::Ping);
            packet << payload;
            _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
        }

        if (!_udpReady) {
            rtp::net::PingPayload payload{};
            rtp::net::Packet packet(rtp::net::OpCode::Ping);
            packet << payload;
            _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::UDP);
            _udpReady = true;
        }
    }

    void NetworkSyncSystem::tryLogin(const std::string& username,
                                     const std::string& password) const
    {
        rtp::net::LoginPayload payload{};
        std::memset(payload.username, 0, sizeof(payload.username));
        std::memset(payload.password, 0, sizeof(payload.password));
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);

        rtp::net::Packet packet(rtp::net::OpCode::LoginRequest);
        packet << payload;
        _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
    }

    void NetworkSyncSystem::tryRegister(const std::string& username,
                                        const std::string& password) const
    {
        rtp::net::RegisterPayload payload{};
        std::memset(payload.username, 0, sizeof(payload.username));
        std::memset(payload.password, 0, sizeof(payload.password));
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);

        rtp::net::Packet packet(rtp::net::OpCode::RegisterRequest);
        packet << payload;
        _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
    }

    void NetworkSyncSystem::requestListRooms(void)
    {
        rtp::net::Packet packet(rtp::net::OpCode::ListRooms);
        _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
    }

    void NetworkSyncSystem::tryCreateRoom(const std::string& roomName,
                                          uint32_t maxPlayers,
                                          float difficulty,
                                          float speed,
                                          uint32_t duration,
                                          uint32_t seed,
                                          uint32_t levelId)
    {
        rtp::net::CreateRoomPayload payload{};
        std::memset(payload.roomName, 0, sizeof(payload.roomName));
        std::strncpy(payload.roomName, roomName.c_str(), sizeof(payload.roomName) - 1);
        payload.maxPlayers = maxPlayers;
        payload.difficulty = difficulty;
        payload.speed = speed;
        payload.duration = duration;
        payload.seed = seed;
        payload.levelId = levelId;

        _currentState = State::CreatingRoom;
        rtp::net::Packet packet(rtp::net::OpCode::CreateRoom);
        packet << payload;
        _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
    }

    void NetworkSyncSystem::tryJoinRoom(uint32_t roomId, bool asSpectator)
    {
        rtp::net::JoinRoomPayload payload{};
        payload.roomId = roomId;
        payload.isSpectator = asSpectator ? 1 : 0;

        _currentState = State::JoiningRoom;
        rtp::net::Packet packet(rtp::net::OpCode::JoinRoom);
        packet << payload;
        _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
    }

    void NetworkSyncSystem::tryLeaveRoom(void)
    {
        rtp::net::Packet packet(rtp::net::OpCode::LeaveRoom);
        _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
        _isInRoom = false;
        _currentState = State::InLobby;
    }

    void NetworkSyncSystem::trySetReady(bool isReady)
    {
        rtp::net::SetReadyPayload payload{};
        payload.isReady = isReady ? 1 : 0;
        _isReady = isReady;

        rtp::net::Packet packet(rtp::net::OpCode::SetReady);
        packet << payload;
        _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
    }

    void NetworkSyncSystem::trySendMessage(const std::string& message) const
    {
        rtp::net::RoomChatPayload payload{};
        std::memset(payload.message, 0, sizeof(payload.message));
        std::strncpy(payload.message, message.c_str(), sizeof(payload.message) - 1);

        rtp::net::Packet packet(rtp::net::OpCode::RoomChatSended);
        packet << payload;
        _network.sendPacket(0, packet.serialize(), engine::net::NetChannel::TCP);
    }

    bool NetworkSyncSystem::isInRoom(void) const { return _isInRoom; }
    bool NetworkSyncSystem::isReady(void) const { return _isReady; }
    bool NetworkSyncSystem::isUdpReady(void) const { return _udpReady; }
    bool NetworkSyncSystem::isLoggedIn(void) const { return _isLoggedIn; }
    std::string NetworkSyncSystem::getUsername(void) const { return _username; }
    std::list<rtp::net::RoomInfo> NetworkSyncSystem::getAvailableRooms(void) const
    {
        return _availableRooms;
    }
    bool NetworkSyncSystem::isInGame(void) const { return _currentState == State::InGame; }
    NetworkSyncSystem::State NetworkSyncSystem::getState(void) const { return _currentState; }
    std::string NetworkSyncSystem::getLastChatMessage(void) const { return _lastChatMessage; }
    const std::deque<std::string>& NetworkSyncSystem::getChatHistory(void) const
    {
        return _chatHistory;
    }
    uint16_t NetworkSyncSystem::getAmmoCurrent(void) const { return _ammoCurrent; }
    uint16_t NetworkSyncSystem::getAmmoMax(void) const { return _ammoMax; }
    bool NetworkSyncSystem::isReloading(void) const { return _ammoReloading; }
    float NetworkSyncSystem::getReloadCooldownRemaining(void) const
    {
        return _ammoReloadRemaining;
    }
    uint32_t NetworkSyncSystem::getPingMs(void) const { return _pingMs; }
    bool NetworkSyncSystem::consumeKicked(void)
    {
        const bool value = _kicked;
        _kicked = false;
        return value;
    }

    /////////////////////////////////////////////////////////////////////////
    // Private API
    /////////////////////////////////////////////////////////////////////////

    void NetworkSyncSystem::handleEvent(engine::net::NetworkEvent& event)
    {
        try {
            auto packet = rtp::net::Packet::deserialize(event.payload);
            if (event.channel == engine::net::NetChannel::UDP) {
                _udpReady = true;
            }
            switch (packet.header.opCode) {
                case rtp::net::OpCode::LoginResponse:
                    onLoginResponse(packet);
                    break;
                case rtp::net::OpCode::RegisterResponse:
                    onRegisterResponse(packet);
                    break;
                case rtp::net::OpCode::RoomList:
                    onRoomResponse(packet);
                    break;
                case rtp::net::OpCode::JoinRoom:
                    onJoinRoomResponse(packet);
                    break;
                case rtp::net::OpCode::CreateRoom:
                    onCreateRoomResponse(packet);
                    break;
                case rtp::net::OpCode::LeaveRoom:
                    onLeaveRoomResponse(packet);
                    break;
                case rtp::net::OpCode::EntitySpawn:
                    onSpawnEntityFromServer(packet);
                    break;
                case rtp::net::OpCode::EntityDeath:
                    onEntityDeath(packet);
                    break;
                case rtp::net::OpCode::RoomUpdate:
                    onRoomUpdate(packet);
                    break;
                case rtp::net::OpCode::RoomChatReceived:
                    onRoomChatReceived(packet);
                    break;
                case rtp::net::OpCode::StartGame:
                    _currentState = State::InGame;
                    break;
                case rtp::net::OpCode::AmmoUpdate:
                    onAmmoUpdate(packet);
                    break;
                case rtp::net::OpCode::Pong:
                    onPong(packet);
                    break;
                case rtp::net::OpCode::DebugModeUpdate:
                    onDebugModeUpdate(packet);
                    break;
                case rtp::net::OpCode::Kicked:
                    _kicked = true;
                    _isLoggedIn = false;
                    _isInRoom = false;
                    _currentState = State::NotLogged;
                    break;
                default:
                    break;
            }
        } catch (const std::exception &e) {
            engine::core::warning("Client: invalid packet: {}", e.what());
        }

        (void)event;
    }

    void NetworkSyncSystem::onLoginResponse(rtp::net::Packet& packet)
    {
        rtp::net::LoginResponsePayload payload{};
        packet >> payload;
        _isLoggedIn = payload.success != 0;
        if (_isLoggedIn) {
            _username = payload.username;
            _currentState = State::InLobby;
        } else {
            _currentState = State::NotLogged;
        }
    }

    void NetworkSyncSystem::onRegisterResponse(rtp::net::Packet& packet)
    {
        rtp::net::RegisterResponsePayload payload{};
        packet >> payload;
        _isLoggedIn = payload.success != 0;
        if (_isLoggedIn) {
            _username = payload.username;
            _currentState = State::InLobby;
        } else {
            _currentState = State::NotLogged;
        }
    }

    void NetworkSyncSystem::onRoomResponse(rtp::net::Packet& packet)
    {
        std::vector<rtp::net::RoomInfo> rooms;
        packet >> rooms;
        _availableRooms.assign(rooms.begin(), rooms.end());
    }

    void NetworkSyncSystem::onJoinRoomResponse(rtp::net::Packet& packet)
    {
        rtp::net::BooleanPayload payload{};
        packet >> payload;
        _isInRoom = payload.status != 0;
        _currentState = _isInRoom ? State::InRoom : State::InLobby;
    }

    void NetworkSyncSystem::onCreateRoomResponse(rtp::net::Packet& packet)
    {
        rtp::net::BooleanPayload payload{};
        packet >> payload;
        if (payload.status == 0) {
            _currentState = State::InLobby;
        }
    }

    void NetworkSyncSystem::onLeaveRoomResponse(rtp::net::Packet&)
    {
        _isInRoom = false;
        _currentState = State::InLobby;
    }

    void NetworkSyncSystem::onSpawnEntityFromServer(rtp::net::Packet& packet)
    {
        rtp::net::EntitySpawnPayload payload{};
        packet >> payload;

        if (_netIdToEntity.contains(payload.netId)) {
            return;
        }

        EntityTemplate t{};
        t.position = {payload.posX, payload.posY};
        t.scale = {1.0f, 1.0f};
        t.sprite.texturePath = "assets/sprites/r-typesheet1.gif";
        t.sprite.rectWidth = static_cast<int>(payload.sizeX);
        t.sprite.rectHeight = static_cast<int>(payload.sizeY);

        auto entity = _builder.spawn(t);
        if (!entity) {
            return;
        }

        _registry.add<engine::ecs::components::NetworkId>(
            entity.value(), engine::ecs::components::NetworkId{payload.netId});
        _netIdToEntity[payload.netId] = entity.value();
    }

    void NetworkSyncSystem::onEntityDeath(rtp::net::Packet& packet)
    {
        rtp::net::EntityDeathPayload payload{};
        packet >> payload;

        auto it = _netIdToEntity.find(payload.netId);
        if (it == _netIdToEntity.end()) {
            return;
        }
        _builder.kill(it->second);
        _netIdToEntity.erase(it);
    }

    void NetworkSyncSystem::onRoomUpdate(rtp::net::Packet& packet)
    {
        rtp::net::RoomSnapshotPayload header{};
        packet >> header;

        std::vector<rtp::net::EntitySnapshotPayload> snapshots;
        packet >> snapshots;

        auto transforms = _registry.getComponents<engine::ecs::components::Transform>();
        auto velocities = _registry.getComponents<engine::ecs::components::Velocity>();

        for (const auto &snapshot : snapshots) {
            auto it = _netIdToEntity.find(snapshot.netId);
            if (it == _netIdToEntity.end()) {
                continue;
            }
            engine::ecs::Entity entity = it->second;

            if (transforms) {
                auto &sa = transforms->get();
                if (entity < sa.size() && sa.has(entity)) {
                    sa[entity].position = snapshot.position;
                    sa[entity].rotation = snapshot.rotation;
                }
            }

            if (velocities) {
                auto &sa = velocities->get();
                if (entity < sa.size() && sa.has(entity)) {
                    auto &vel = sa[entity];
                    vel.direction = snapshot.velocity;
                    vel.speed = 1.0f;
                }
            }
        }

        _currentState = header.inGame ? State::InGame : _currentState;
    }

    void NetworkSyncSystem::onRoomChatReceived(rtp::net::Packet& packet)
    {
        rtp::net::RoomChatReceivedPayload payload{};
        packet >> payload;
        _lastChatMessage = payload.message;
        pushChatMessage(payload.message);
    }

    void NetworkSyncSystem::pushChatMessage(const std::string& message)
    {
        _chatHistory.push_back(message);
        if (_chatHistory.size() > _chatHistoryLimit) {
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
        auto now = std::chrono::time_point_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now());
        const uint64_t ms =
            static_cast<uint64_t>(now.time_since_epoch().count());
        _pingMs = static_cast<uint32_t>(ms - payload.clientTimeMs);
    }

    void NetworkSyncSystem::onDebugModeUpdate(rtp::net::Packet&)
    {
    }
} // namespace rtp::client
