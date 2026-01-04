/**
 * File   : INetwork.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/GameManager.hpp"
#include "RType/Logger.hpp"
#include <thread>
#include <chrono>

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
        
        auto lobby = std::make_shared<Room>(_nextRoomId++, "Global Lobby", 9999, 0, 0, Room::RoomType::Lobby);
        _rooms[lobby->getId()] = lobby;
        _lobbyId = lobby->getId();
        log::info("Default Lobby created with ID {}", lobby->getId());
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
            _serverNetworkSystem->broadcastWorldState(_serverTick);
            
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
        log::info("Player with Session ID {} connected", sessionId);

        uint32_t res = _serverNetworkSystem->handlePlayerConnection(sessionId, net::Packet());
        if (res != sessionId) {
            log::error("Session ID mismatch on player connection");
            return;
        }
        auto player = std::make_shared<Player>(sessionId);
        
        {
            std::lock_guard lock(_mutex);
            _players[sessionId] = player;
        }
        rtp::log::info("Player with Session ID {} added to player list", sessionId);
    }

    void GameManager::handlePlayerDisconnect(uint32_t sessionId)
    {
        log::info("Player with Session ID {} disconnected", sessionId);

        uint32_t entityNetId = 0;

        {
            std::lock_guard lock(_mutex);

            auto itP = _players.find(sessionId);
            if (itP != _players.end()) {
                entityNetId = itP->second->getEntityId();
            }

            _players.erase(sessionId);
            
            auto it = _playerRoomMap.find(sessionId);

            if (it != _playerRoomMap.end()) {
                uint32_t roomId = it->second;
                auto roomIt = _rooms.find(roomId);
                if (roomIt != _rooms.end()) {
                    roomIt->second->removePlayer(sessionId);
                    sendRoomUpdate(*roomIt->second);
                }
                _playerRoomMap.erase(it);
            }
            _serverNetworkSystem->handleDisconnect(sessionId);
        }

        if (entityNetId != 0) {
            rtp::net::Packet disconnectPlayer(rtp::net::OpCode::Disconnect);
            disconnectPlayer << entityNetId;
            _networkManager.broadcastPacket(disconnectPlayer, rtp::net::NetworkMode::UDP);
        }
    }

    void GameManager::handlePlayerLoginAuth(uint32_t sessionId, const Packet &packet)
    {
        rtp::log::info("Handling authentication for Session ID {}", sessionId);
        auto res = _authSystem->handleLoginRequest(sessionId, packet);
        if (!res.first) return;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            auto it = _players.find(sessionId);
            if (it == _players.end()) return;
            it->second->setUsername(res.second);
            player = it->second;
        }

        tryJoinRoom(player, _lobbyId);
        player->setState(PlayerState::InLobby);
    }

    void GameManager::handlePlayerRegisterAuth(uint32_t sessionId, const Packet &packet)
    {
        rtp::log::info("Handling registration for Session ID {}", sessionId);
        auto res = _authSystem->handleRegisterRequest(sessionId, packet);
        if (!res.first) return;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            auto it = _players.find(sessionId);
            if (it == _players.end()) return;
            it->second->setUsername(res.second);
            player = it->second;
        }

        tryJoinRoom(player, _lobbyId);
        player->setState(PlayerState::InLobby);
    }

    void GameManager::handleListRooms(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        log::info("Handle List Rooms request from Session ID {}", sessionId);

        rtp::net::Packet responsePacket(rtp::net::OpCode::RoomList);
        uint32_t roomCount = static_cast<uint32_t>(_rooms.size());
        responsePacket << roomCount;

        for (const auto& [roomId, roomPtr] : _rooms) {
            rtp::net::RoomInfo roomInfo;
            roomInfo.roomId = roomPtr->getId();
            std::strncpy(roomInfo.roomName, roomPtr->getName().c_str(), sizeof(roomInfo.roomName));
            roomInfo.currentPlayers = roomPtr->getCurrentPlayerCount();
            roomInfo.maxPlayers = roomPtr->getMaxPlayers();
            roomInfo.difficulty = roomPtr->getDifficulty();
            roomInfo.speed = roomPtr->getSpeed();
            responsePacket << roomInfo;
        }

        _networkManager.sendPacket(sessionId, responsePacket, rtp::net::NetworkMode::TCP);
    }

    void GameManager::handleCreateRoom(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        
        rtp::net::CreateRoomPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("Handle Create Room request from Session ID {}: Name='{}', MaxPlayers={}",
                  sessionId, payload.roomName, payload.maxPlayers);

        auto roomRes = _rooms.emplace(
            _nextRoomId,
            std::make_shared<Room>(_nextRoomId, payload.roomName, payload.maxPlayers, payload.difficulty, payload.speed, Room::RoomType::Public)
        );
        if (!roomRes.second) {
            log::error("Failed to create new room for Session ID {}", sessionId);
            return;
        }
        tryJoinRoom(_players[sessionId], _nextRoomId);
        log::info("Room '{}' created with ID {} by Session ID {}",
                payload.roomName, _nextRoomId, sessionId);
        _nextRoomId++;
    }

    void GameManager::handleJoinRoom(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        
        rtp::net::JoinRoomPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("Handle Join Room request from Session ID {}: Room ID {}",
                  sessionId, payload.roomId);
        tryJoinRoom(_players[sessionId], payload.roomId);
    }

    void GameManager::handleLeaveRoom(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        
        rtp::net::LeaveRoomPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("Handle Leave Room request from Session ID {}: Room ID {}",
                  sessionId, payload.roomId);
        tryJoinRoom(_players[sessionId], _lobbyId);
        auto it = _rooms.find(payload.roomId);
        if (it != _rooms.end()) {
            it->second->removePlayer(sessionId);
            _playerRoomMap.erase(sessionId);
            sendRoomUpdate(*it->second);
        }
    }

    void GameManager::handleSetReady(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        
        rtp::net::SetReadyPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("Handle Set Ready request from Session ID {}: isReady={}",
                  sessionId, payload.isReady);
        _players[sessionId]->setReady(payload.isReady != 0);
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

    // void GameManager::tryJoinLobby(PlayerPtr player, uint32_t roomId)
    // {
    //     std::lock_guard lock(_mutex);

    //     std::shared_ptr<Room> targetRoom = nullptr;
    //     if (roomId == 0) {
    //         if (auto it = _rooms.find(1); it != _rooms.end() && it->second->canJoin() && it->second->getType() == Room::Type::Lobby) {
    //              targetRoom = it->second;
    //         }
    //     }

    //     if (targetRoom != nullptr) {
    //         targetRoom->addPlayer(player);
    //         _playerRoomMap[player->getId()] = targetRoom->getId();
            
    //         player->setRoomId(targetRoom->getId());
    //         player->setState(PlayerState::InLobby);

    //         log::info("Player {} (Session ID {}) joined Room ID {}", 
    //                   player->getUsername(), player->getId(), targetRoom->getId());
            
    //         rtp::net::Packet welcomePacket(rtp::net::OpCode::Welcome);
    //         welcomePacket << player->getId();
    //         _networkManager.sendPacket(player->getId(), welcomePacket, rtp::net::NetworkMode::TCP);
            
    //         std::this_thread::yield();

    //         sendLobbyUpdate(*targetRoom);

    //         spawnPlayerEntity(player);

    //         uint32_t entityId = player->getEntityId();
    //         _serverNetworkSystem->broadcastWorldState(_serverTick);

    //     } else {
    //          log::error("Failed to join Player {} to any room. Lobby is full.", player->getId());
    //     }
    // }

    void GameManager::sendRoomUpdate(const Room &room)
    {
        rtp::net::Packet updatePacket(rtp::net::OpCode::RoomUpdate);
        for (const auto& player : room.getPlayers()) {
            _networkManager.sendPacket(player->getId(), updatePacket, rtp::net::NetworkMode::TCP);
        }
    }

    void GameManager::tryJoinRoom(PlayerPtr player, uint32_t roomId)
    {
        std::shared_ptr<Room> room;

        {
            std::lock_guard lock(_mutex);

            auto it = _rooms.find(roomId);
            if (it == _rooms.end()) {
                log::error("Failed to join Player {} to Room ID {}. Room does not exist.",
                        player->getId(), roomId);
                return;
            }

            room = it->second;

            if (_playerRoomMap.find(player->getId()) != _playerRoomMap.end()) {
                log::warning("Player {} already in a room", player->getId());
                return;
            }

            if (!room->canJoin()) {
                log::error("Failed to join Player {} to Room ID {}. Room is full.",
                        player->getId(), roomId);
                return;
            }

            if (!room->addPlayer(player)) {
                log::error("Failed to add Player {} to Room ID {}", player->getId(), roomId);
                return;
            }

            _playerRoomMap[player->getId()] = room->getId();
            player->setRoomId(room->getId());
            player->setState(PlayerState::InLobby);

            log::info("Player {} (Session ID {}) joined Room ID {}",
                    player->getUsername(), player->getId(), room->getId());
        }

        rtp::net::Packet welcomePacket(rtp::net::OpCode::Welcome);
        welcomePacket << player->getId();
        _networkManager.sendPacket(player->getId(), welcomePacket, rtp::net::NetworkMode::TCP);

        std::this_thread::yield();

        sendRoomUpdate(*room);

        spawnPlayerEntity(player);

        _serverNetworkSystem->broadcastWorldState(_serverTick);
    }
}