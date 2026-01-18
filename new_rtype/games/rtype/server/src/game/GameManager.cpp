/**
 * File   : GameManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/game/GameManager.hpp"

#include "engine/log/Logger.hpp"

#include <cstring>

namespace rtp::server
{
    namespace
    {
        constexpr uint8_t kMoveUp = 1 << 0;
        constexpr uint8_t kMoveDown = 1 << 1;
        constexpr uint8_t kMoveLeft = 1 << 2;
        constexpr uint8_t kMoveRight = 1 << 3;
        constexpr float kBaseSpeed = 200.f;
        constexpr uint8_t kEntityTypePlayer = 1;
    }

    /////////////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////////////

    GameManager::GameManager(aer::net::INetwork &network)
        : _network(network),
          _authSystem(network),
          _playerSystem(),
          _roomSystem(network, _playerSystem)
    {
        _roomSystem.setOnRoomStarted([this](uint32_t roomId) {
            spawnRoomEntities(roomId);
        });
    }

    void GameManager::handleEvent(const aer::net::NetworkEvent &event)
    {
        try {
            auto packet = rtp::net::Packet::deserialize(event.payload);
            handlePacket(event.sessionId, event.channel, std::move(packet));
        } catch (const std::exception &e) {
            aer::log::warning("Server: failed to parse packet: {}", e.what());
        }
    }

    void GameManager::update(float dt)
    {
        _serverTick++;
        _roomSystem.launchReadyRooms(dt);
        broadcastRoomState(dt);
    }

    /////////////////////////////////////////////////////////////////////////
    // Private API
    /////////////////////////////////////////////////////////////////////////

    void GameManager::handlePacket(uint32_t sessionId,
                                   aer::net::NetChannel channel,
                                   rtp::net::Packet packet)
    {
        switch (packet.header.opCode) {
            case rtp::net::OpCode::Hello:
                handlePlayerConnect(sessionId);
                break;
            case rtp::net::OpCode::Disconnect:
                handlePlayerDisconnect(sessionId);
                break;
            case rtp::net::OpCode::LoginRequest:
                handleLogin(sessionId, packet);
                break;
            case rtp::net::OpCode::RegisterRequest:
                handleRegister(sessionId, packet);
                break;
            case rtp::net::OpCode::ListRooms:
                handleListRooms(sessionId);
                break;
            case rtp::net::OpCode::CreateRoom:
                handleCreateRoom(sessionId, packet);
                break;
            case rtp::net::OpCode::JoinRoom:
                handleJoinRoom(sessionId, packet);
                break;
            case rtp::net::OpCode::LeaveRoom:
                handleLeaveRoom(sessionId);
                break;
            case rtp::net::OpCode::SetReady:
                handleSetReady(sessionId, packet);
                break;
            case rtp::net::OpCode::RoomChatSended:
                handleRoomChat(sessionId, packet);
                break;
            case rtp::net::OpCode::InputTick:
                handleInputTick(sessionId, packet);
                break;
            case rtp::net::OpCode::Ping:
                handlePing(sessionId, packet);
                break;
            default:
                aer::core::debug("Server: unhandled opcode {}", static_cast<uint32_t>(packet.header.opCode));
                break;
        }

        (void)channel;
    }

    void GameManager::handlePlayerConnect(uint32_t sessionId)
    {
        aer::log::info("Client with Session ID {} connected", sessionId);
    }

    void GameManager::handlePlayerDisconnect(uint32_t sessionId)
    {
        auto player = _playerSystem.getPlayer(sessionId);
        const uint32_t roomId = player ? player->roomId() : 0;

        if (player) {
            handleLeaveRoom(sessionId);
        }

        _playerSystem.removePlayer(sessionId);

        if (roomId != 0) {
            broadcastEntityDeath(roomId, sessionId);
        }
    }

    void GameManager::handleLogin(uint32_t sessionId, rtp::net::Packet &packet)
    {
        const auto [success, username] = _authSystem.handleLoginRequest(sessionId, packet);
        if (!success) {
            return;
        }
        auto player = _playerSystem.createPlayer(sessionId, username);
        player->setLoggedIn(true);
        _roomSystem.joinLobby(player);
    }

    void GameManager::handleRegister(uint32_t sessionId, rtp::net::Packet &packet)
    {
        const auto [success, username] = _authSystem.handleRegisterRequest(sessionId, packet);
        if (!success) {
            return;
        }
        auto player = _playerSystem.createPlayer(sessionId, username);
        player->setLoggedIn(true);
        _roomSystem.joinLobby(player);
    }

    void GameManager::handleListRooms(uint32_t sessionId)
    {
        _roomSystem.listAllRooms(sessionId);
    }

    void GameManager::handleCreateRoom(uint32_t sessionId, rtp::net::Packet &packet)
    {
        rtp::net::CreateRoomPayload payload{};
        packet >> payload;

        auto player = _playerSystem.getPlayer(sessionId);
        rtp::net::Packet response(rtp::net::OpCode::CreateRoom);
        if (!player) {
            response << rtp::net::BooleanPayload{0};
            const auto data = response.serialize();
            _network.sendPacket(sessionId, data, aer::net::NetChannel::TCP);
            return;
        }

        const uint32_t roomId = _roomSystem.createRoom(sessionId,
                                                      payload.roomName,
                                                      static_cast<uint8_t>(payload.maxPlayers),
                                                      payload.difficulty,
                                                      payload.speed,
                                                      Room::RoomType::Public,
                                                      payload.levelId,
                                                      payload.seed,
                                                      payload.duration);
        const bool joined = _roomSystem.joinRoom(player, roomId, false);
        response << rtp::net::BooleanPayload{static_cast<uint8_t>(joined ? 1 : 0)};
        const auto data = response.serialize();
        _network.sendPacket(sessionId, data, aer::net::NetChannel::TCP);
    }

    void GameManager::handleJoinRoom(uint32_t sessionId, rtp::net::Packet &packet)
    {
        rtp::net::JoinRoomPayload payload{};
        packet >> payload;

        auto player = _playerSystem.getPlayer(sessionId);
        if (!player) {
            return;
        }

        const bool success = _roomSystem.joinRoom(player, payload.roomId, payload.isSpectator != 0);
        if (!success) {
            return;
        }

        auto room = _roomSystem.getRoom(payload.roomId);
        if (!room) {
            return;
        }
        if (room->state() == Room::State::InGame) {
            rtp::net::Packet startPacket(rtp::net::OpCode::StartGame);
            const auto data = startPacket.serialize();
            _network.sendPacket(sessionId, data, aer::net::NetChannel::TCP);
            sendRoomEntitiesToSession(payload.roomId, sessionId);
        }
    }

    void GameManager::handleLeaveRoom(uint32_t sessionId)
    {
        auto player = _playerSystem.getPlayer(sessionId);
        if (!player) {
            return;
        }
        const uint32_t roomId = player->roomId();
        const uint32_t entityId = player->entityId();
        _roomSystem.leaveRoom(player);
        player->setEntityId(0);

        if (roomId != 0 && entityId != 0) {
            broadcastEntityDeath(roomId, sessionId);
        }
    }

    void GameManager::handleSetReady(uint32_t sessionId, rtp::net::Packet &packet)
    {
        rtp::net::SetReadyPayload payload{};
        packet >> payload;

        auto player = _playerSystem.getPlayer(sessionId);
        if (!player) {
            return;
        }
        player->setReady(payload.isReady != 0);
    }

    void GameManager::handleRoomChat(uint32_t sessionId, rtp::net::Packet &packet)
    {
        auto player = _playerSystem.getPlayer(sessionId);
        if (!player || player->roomId() == 0) {
            return;
        }

        rtp::net::RoomChatPayload payload{};
        packet >> payload;

        rtp::net::RoomChatReceivedPayload out{};
        out.sessionId = sessionId;
        std::memset(out.username, 0, sizeof(out.username));
        std::memset(out.message, 0, sizeof(out.message));
        std::strncpy(out.username, player->username().c_str(), sizeof(out.username) - 1);
        std::strncpy(out.message, payload.message, sizeof(out.message) - 1);

        rtp::net::Packet response(rtp::net::OpCode::RoomChatReceived);
        response << out;
        const auto data = response.serialize();

        auto room = _roomSystem.getRoom(player->roomId());
        if (!room) {
            return;
        }
        for (const auto sid : room->players()) {
            _network.sendPacket(sid, data, aer::net::NetChannel::TCP);
        }
    }

    void GameManager::handleInputTick(uint32_t sessionId, rtp::net::Packet &packet)
    {
        rtp::net::InputPayload payload{};
        packet >> payload;

        auto player = _playerSystem.getPlayer(sessionId);
        if (!player) {
            return;
        }
        player->setInputMask(payload.inputMask);
    }

    void GameManager::handlePing(uint32_t sessionId, rtp::net::Packet &packet)
    {
        rtp::net::PingPayload payload{};
        packet >> payload;

        rtp::net::Packet response(rtp::net::OpCode::Pong);
        response << payload;
        const auto data = response.serialize();
        _network.sendPacket(sessionId, data, aer::net::NetChannel::TCP);
    }

    void GameManager::spawnRoomEntities(uint32_t roomId)
    {
        auto room = _roomSystem.getRoom(roomId);
        if (!room) {
            return;
        }

        for (const auto sessionId : room->players()) {
            auto player = _playerSystem.getPlayer(sessionId);
            if (!player) {
                continue;
            }
            if (player->entityId() == 0) {
                player->setEntityId(_nextEntityId++);
                player->setPosition({100.f, 100.f});
            }
        }

        for (const auto sessionId : room->players()) {
            sendRoomEntitiesToSession(roomId, sessionId);
        }
    }

    void GameManager::sendRoomEntitiesToSession(uint32_t roomId, uint32_t sessionId)
    {
        auto room = _roomSystem.getRoom(roomId);
        if (!room) {
            return;
        }

        for (const auto sid : room->players()) {
            auto player = _playerSystem.getPlayer(sid);
            if (!player || player->entityId() == 0) {
                continue;
            }

            rtp::net::EntitySpawnPayload payload{};
            payload.netId = player->entityId();
            payload.type = kEntityTypePlayer;
            payload.posX = player->position().x;
            payload.posY = player->position().y;
            payload.sizeX = 32.f;
            payload.sizeY = 16.f;

            rtp::net::Packet packet(rtp::net::OpCode::EntitySpawn);
            packet << payload;
            const auto data = packet.serialize();
            _network.sendPacket(sessionId, data, aer::net::NetChannel::TCP);
        }
    }

    void GameManager::broadcastEntityDeath(uint32_t roomId, uint32_t sessionId)
    {
        auto room = _roomSystem.getRoom(roomId);
        if (!room) {
            return;
        }

        auto player = _playerSystem.getPlayer(sessionId);
        if (!player) {
            return;
        }

        rtp::net::EntityDeathPayload payload{};
        payload.netId = player->entityId();
        payload.type = kEntityTypePlayer;
        payload.position = player->position();

        rtp::net::Packet packet(rtp::net::OpCode::EntityDeath);
        packet << payload;
        const auto data = packet.serialize();

        for (const auto sid : room->players()) {
            _network.sendPacket(sid, data, aer::net::NetChannel::TCP);
        }
    }

    void GameManager::broadcastRoomState(float dt)
    {
        const auto rooms = _roomSystem.rooms();
        for (const auto &room : rooms) {
            if (!room || room->type() == Room::RoomType::Lobby) {
                continue;
            }
            if (room->state() != Room::State::InGame) {
                continue;
            }

            const float speed = kBaseSpeed * room->speed();
            std::vector<rtp::net::EntitySnapshotPayload> snapshots;
            snapshots.reserve(room->players().size());

            for (const auto sid : room->players()) {
                auto player = _playerSystem.getPlayer(sid);
                if (!player || player->entityId() == 0) {
                    continue;
                }

                aer::math::Vec2f velocity{0.f, 0.f};
                const uint8_t mask = player->inputMask();
                if (mask & kMoveUp) {
                    velocity.y -= speed;
                }
                if (mask & kMoveDown) {
                    velocity.y += speed;
                }
                if (mask & kMoveLeft) {
                    velocity.x -= speed;
                }
                if (mask & kMoveRight) {
                    velocity.x += speed;
                }

                auto pos = player->position();
                pos.x += velocity.x * dt;
                pos.y += velocity.y * dt;
                player->setPosition(pos);
                player->setVelocity(velocity);

                snapshots.push_back({
                    player->entityId(),
                    player->position(),
                    player->velocity(),
                    0.0f
                });
            }

            if (snapshots.empty()) {
                continue;
            }

            rtp::net::RoomSnapshotPayload header{};
            header.roomId = room->id();
            header.currentPlayers = static_cast<uint32_t>(room->playerCount());
            header.serverTick = _serverTick;
            header.entityCount = static_cast<uint16_t>(snapshots.size());
            header.inGame = room->state() == Room::State::InGame ? 1 : 0;

            rtp::net::Packet packet(rtp::net::OpCode::RoomUpdate);
            packet << header << snapshots;
            const auto data = packet.serialize();

            for (const auto sid : room->players()) {
                _network.sendPacket(sid, data, aer::net::NetChannel::UDP);
            }
        }
    }
}
