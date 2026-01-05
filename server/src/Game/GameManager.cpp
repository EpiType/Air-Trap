/**
 * File   : GameManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/GameManager.hpp"

using namespace rtp::net;

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    GameManager::GameManager(ServerNetwork &networkManager)
        : _networkManager(networkManager)
    {        
        _registry.registerComponent<rtp::ecs::components::Transform>();
        _registry.registerComponent<rtp::ecs::components::Velocity>();
        _registry.registerComponent<rtp::ecs::components::NetworkId>();
        _registry.registerComponent<rtp::ecs::components::server::InputComponent>();
        _registry.registerComponent<rtp::ecs::components::EntityType>();

        _serverNetworkSystem = std::make_unique<ServerNetworkSystem>(_networkManager, _registry);
        _movementSystem = std::make_unique<MovementSystem>(_registry);
        _authSystem = std::make_unique<AuthSystem>(_networkManager, _registry);
        _roomSystem =  std::make_unique<RoomSystem>(_networkManager, _registry);
        _playerSystem = std::make_unique<PlayerSystem>(_networkManager, _registry);
        
        log::info("GameManager initialized");
    }

    GameManager::~GameManager()
    {
        log::info("GameManager destroyed");
    }

    void GameManager::gameLoop(void)
    {
        const float dt = 1.0f / 60.0f;

        while (true) {
            auto start = std::chrono::high_resolution_clock::now();

            processNetworkEvents();

            _serverTick++;
            _movementSystem->update(dt);
            _serverNetworkSystem->broadcastRoomState(_serverTick);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void startGame(Room &room)
    {
        log::info("Starting game in Room ID {}", room.getId());
        /* Future feature */
    }

    //////////////////////////////////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////////////////////////////////

    void GameManager::processNetworkEvents(void)
    {
        while (auto optionalEvent = _networkManager.pollEvent()) {
            auto& event = optionalEvent.value();
            switch (event.packet.header.opCode) {
                case OpCode::None:
                    break;
                case OpCode::Hello:
                    handlePlayerConnect(event.sessionId);
                    break;
                case OpCode::Disconnect:
                    handlePlayerDisconnect(event.sessionId);
                    break;
                case OpCode::LoginRequest:
                    handlePlayerLoginAuth(event.sessionId, event.packet);
                    break;
                case OpCode::RegisterRequest:
                    handlePlayerRegisterAuth(event.sessionId, event.packet);
                    break;
                case OpCode::InputTick:
                    _serverNetworkSystem->handleInput(event.sessionId, event.packet);
                    break;
                case OpCode::ListRooms:
                    handleListRooms(event.sessionId, event.packet);
                    break;
                case OpCode::CreateRoom:
                    handleCreateRoom(event.sessionId, event.packet);
                    break;
                case OpCode::JoinRoom:
                    handleJoinRoom(event.sessionId, event.packet);
                    break;
                case OpCode::LeaveRoom:
                    handleLeaveRoom(event.sessionId, event.packet);
                    break;
                case OpCode::SetReady:
                    handleSetReady(event.sessionId, event.packet);
                    break;
                case OpCode::RoomChatSended:
                    handleRoomChatSended(event.sessionId, event.packet);
                    break;
                default:
                    handlePacket(event.sessionId, event.packet);
                    break;
            }
        }
    }

    void GameManager::handlePlayerConnect(uint32_t sessionId)
    {
        log::info("Client with Session ID {} connected, not logged yet", sessionId);
    }

    void GameManager::handlePlayerDisconnect(uint32_t sessionId)
    {
        uint32_t entityId = 0;

        {
            std::lock_guard lock(_mutex);
            entityId = _playerSystem->removePlayer(sessionId);
            _roomSystem->disconnectPlayer(sessionId);
            _serverNetworkSystem->handleDisconnect(sessionId);
        }

        if (entityId != 0) {
            rtp::net::Packet disconnectPlayer(rtp::net::OpCode::Disconnect);
            disconnectPlayer << entityId;
            _networkManager.broadcastPacket(disconnectPlayer, rtp::net::NetworkMode::UDP);
        }

        log::info("Player with Session ID {} disconnected", sessionId);
    }

    void GameManager::handlePlayerLoginAuth(uint32_t sessionId, const Packet &packet)
    {
        rtp::log::info("Handling authentication for Session ID {}", sessionId);
        auto res = _authSystem->handleLoginRequest(sessionId, packet);
        if (!res.first) return;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->createPlayer(sessionId, res.second);
        }

        if (_roomSystem->joinLobby(player)) {
            player->setState(PlayerState::InLobby);
        } else {
            rtp::log::error("Failed to join Lobby for Session ID {}", sessionId);
        }
    }

    void GameManager::handlePlayerRegisterAuth(uint32_t sessionId, const Packet &packet)
    {
        rtp::log::info("Handling registration for Session ID {}", sessionId);
        auto res = _authSystem->handleRegisterRequest(sessionId, packet);
        if (!res.first) return;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->createPlayer(sessionId, res.second);
        }

        if (_roomSystem->joinLobby(player)) {
            player->setState(PlayerState::InLobby);
        } else {
            rtp::log::error("Failed to join Lobby for Session ID {}", sessionId);
        }
    }

    void GameManager::handleListRooms(uint32_t sessionId, const Packet &packet)
    {
        _roomSystem->listAllRooms(sessionId, packet);
    }

    void GameManager::handleCreateRoom(uint32_t sessionId, const Packet &packet)
    {
        CreateRoomPayload payload;
        Packet tmp = packet;
        tmp >> payload;

        PlayerPtr player;
        uint32_t newId = 0;

        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->getPlayer(sessionId);
            newId = _roomSystem->createRoom(
                sessionId,
                std::string(payload.roomName),
                static_cast<uint8_t>(payload.maxPlayers),
                payload.difficulty,
                payload.speed,
                Room::RoomType::Public
            );
        }
        rtp::log::info("Handle Create Room request from Session ID {}: Room Name '{}', Max Players {}, Difficulty {}, Speed {}",
                   sessionId, payload.roomName, payload.maxPlayers, payload.difficulty, payload.speed);
        _roomSystem->joinRoom(player, newId);
    }

    void GameManager::handleJoinRoom(uint32_t sessionId, const Packet &packet)
    {        
        rtp::net::JoinRoomPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->getPlayer(sessionId);
        }

        rtp::log::info("Handle Join Room request from Session ID {}: Room ID {}",
                   sessionId, payload.roomId);

        _roomSystem->joinRoom(player, payload.roomId);
    }

    void GameManager::handleLeaveRoom(uint32_t sessionId, const Packet &packet)
    {
        (void)packet;
        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->getPlayer(sessionId);
        }

        rtp::log::info("Handle Leave Room request from Session ID {}", sessionId);

        _roomSystem->leaveRoom(player);
    }

    void GameManager::handleSetReady(uint32_t sessionId, const Packet &packet)
    {
        // std::lock_guard lock(_mutex);
        
        // rtp::net::SetReadyPayload payload;
        // rtp::net::Packet tempPacket = packet;
        // tempPacket >> payload;
        // rtp::log::info("Handle Set Ready request from Session ID {}: isReady={}",
        //           sessionId, payload.isReady);
        // _players[sessionId]->setReady(payload.isReady != 0);
    }

    void GameManager::handleRoomChatSended(uint32_t sessionId, const Packet &packet)
    {
        // std::lock_guard lock(_mutex);
        
        // rtp::net::RoomChatSendedPayload payload;
        // rtp::net::Packet tempPacket = packet;
        // tempPacket >> payload;
        // rtp::log::info("Handle Room Chat from Session ID {}: Message='{}'",
        //           sessionId, payload.message);
        // auto it = _playerRoomMap.find(sessionId);
        // if (it != _playerRoomMap.end()) {
        //     uint32_t roomId = it->second;
        //     auto roomIt = _rooms.find(roomId);
        //     if (roomIt != _rooms.end()) {
        //         rtp::net::Packet chatPacket(rtp::net::OpCode::RoomChatReceived);
        //         rtp::net::RoomChatReceivedPayload chatPayload;
        //         chatPayload.sessionId = sessionId;
        //         std::strncpy(chatPayload.message, payload.message, sizeof(chatPayload.message));
        //         chatPacket << chatPayload;
        //         roomIt->second->broadcastPacket(chatPacket, _networkManager, rtp::net::NetworkMode::TCP);
        //     }
        // }
    }

    void GameManager::handlePacket(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        rtp::log::info("Received packet OpCode {} from Session {}", (int)rtp::net::OpCode::LoginRequest, sessionId);
        log::debug("Unknown packet received OpCode {} from Session {}", (int)packet.header.opCode, sessionId);
    }

    void GameManager::spawnPlayerEntity(PlayerPtr player)
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            log::error("Failed to spawn entity for player {}", player->getId());
            return;
        }
        log::info("Spawning entity for player {}", player->getId());
        auto entity = entityRes.value();

        _registry.addComponent<rtp::ecs::components::Transform>(
            entity, 
            rtp::ecs::components::Transform{ {100.f, 100.f}, 0.f, {1.f, 1.f} }
        );

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            entity, 
            rtp::ecs::components::NetworkId{ (uint32_t)entity }
        );

        _registry.addComponent<rtp::ecs::components::server::InputComponent>(
            entity, 
            rtp::ecs::components::server::InputComponent{}
        );

        _registry.addComponent<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ rtp::net::EntityType::Player }
        );
        
        player->setEntityId((uint32_t)entity);
        _serverNetworkSystem->bindSessionToEntity(player->getId(), (uint32_t)entity);

        log::info("Spawned Entity {} for Player {}", (uint32_t)entity, player->getId());
    }

    void GameManager::spawnEnemyEntity(const Vec2f& position)
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            log::error("Failed to spawn enemy entity");
            return;
        }
        log::info("Spawning enemy entity");
        auto entity = entityRes.value();

        _registry.addComponent<rtp::ecs::components::Transform>(
            entity, 
            rtp::ecs::components::Transform{ position, 0.f, {1.f, 1.f} }
        );

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            entity, 
            rtp::ecs::components::NetworkId{ (uint32_t)entity }
        );

        _registry.addComponent<rtp::ecs::components::Velocity>(
            entity, 
            rtp::ecs::components::Velocity{ {-100.f, 0.f} }
        );

        _registry.addComponent<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ rtp::net::EntityType::Scout }
        );

        rtp::net::Packet spawnEnemyPacket(rtp::net::OpCode::EntitySpawn);

        rtp::net::EntitySpawnPayload payload{
            .netId    = (uint32_t)entity,
            .type     = static_cast<uint8_t>(rtp::net::EntityType::Scout),
            .position = position
        };

        spawnEnemyPacket << payload;

        _networkManager.broadcastPacket(spawnEnemyPacket, rtp::net::NetworkMode::UDP);

        log::info("Spawned Enemy Entity {}", (uint32_t)entity);
    }

    void GameManager::sendRoomUpdate(const Room &room)
    {
        rtp::net::Packet updatePacket(rtp::net::OpCode::RoomUpdate);
        for (const auto& player : room.getPlayers()) {
            _networkManager.sendPacket(player->getId(), updatePacket, rtp::net::NetworkMode::TCP);
        }
    }

    void GameManager::tryJoinRoom(PlayerPtr player, uint32_t roomId)
    {
        try {
            _roomSystem->joinRoom(player, roomId);
            std::this_thread::yield();
            spawnPlayerEntity(player);
            _serverNetworkSystem->broadcastRoomState(_serverTick);
        } catch (const std::exception &e) {
            log::error("Exception in tryJoinRoom for Player {}: {}", player->getId(), e.what());
        }
    }
}