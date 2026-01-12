/**
 * File   : GameManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/GameManager.hpp"
#include "RType/ECS/Components/RoomId.hpp"

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
        _registry.registerComponent<rtp::ecs::components::RoomId>();

        _networkSyncSystem = std::make_unique<NetworkSyncSystem>(_networkManager, _registry);
        _movementSystem = std::make_unique<MovementSystem>(_registry);
        _authSystem = std::make_unique<AuthSystem>(_networkManager, _registry);
        _roomSystem =  std::make_unique<RoomSystem>(_networkManager, _registry, *_networkSyncSystem);
        _playerSystem = std::make_unique<PlayerSystem>(_networkManager, _registry);
        _entitySystem = std::make_unique<EntitySystem>(_registry, _networkManager, *_networkSyncSystem);
        _playerMouvementSystem = std::make_unique<PlayerMouvementSystem>(_registry);

        _roomSystem->setOnRoomStarted(
            [this](uint32_t roomId) {
                auto room = _roomSystem->getRoom(roomId);
                if (!room) {
                    rtp::log::error("Room ID {} not found for start event", roomId);
                    return;
                }

                const auto players = room->getPlayers();
                std::vector<uint32_t> sessions;
                sessions.reserve(players.size());
                for (const auto& player : players) {
                    sessions.push_back(player->getId());
                }

                for (auto& player : players) {
                    auto entity = _entitySystem->createPlayerEntity(player);
                    uint32_t entityId = static_cast<uint32_t>(entity.index());
                    player->setEntityId(entityId);
                    rtp::log::info("Spawned Entity {} for Player {}", entity.index(), player->getId());
                    _networkSyncSystem->bindSessionToEntity(player->getId(), entityId);
                    sendEntitySpawnToSessions(entity, sessions);
                }

                for (int i = 0; i < 5; ++i) {
                    auto entity = _entitySystem->creaetEnemyEntity(roomId, {120.f + i * 15.f, 200.f + i * 8.f}, 
                        rtp::ecs::components::Patterns::SineWave, 200.0f, 40.0f, 2.0f);
                    rtp::log::info("Spawned Enemy Entity {} in room {}", entity.index(), roomId);
                    sendEntitySpawnToSessions(entity, sessions);
                }
            }
        );


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
            _roomSystem->update(dt);
            _playerMouvementSystem->update(dt);
            _movementSystem->update(dt);
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
                    _networkSyncSystem->handleInput(event.sessionId, event.packet);
                    break;
                case OpCode::ListRooms:
                    handleListRooms(event.sessionId);
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
            _networkSyncSystem->handleDisconnect(sessionId);
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

    void GameManager::handleListRooms(uint32_t sessionId)
    {
        _roomSystem->listAllRooms(sessionId);
    }

    void GameManager::handleCreateRoom(uint32_t sessionId, const Packet &packet)
    {
        CreateRoomPayload payload;
        Packet tmp = packet;
        tmp >> payload;

        PlayerPtr player;
        uint32_t newId = 0;
        uint8_t success = 0;
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
            success = (newId != 0) ? 1 : 0;
        }
        if (success) {
            success = _roomSystem->joinRoom(player, newId) ? 1 : 0;
        }
        rtp::net::Packet response(rtp::net::OpCode::CreateRoom);
        response << success;
        _networkManager.sendPacket(sessionId, response, rtp::net::NetworkMode::TCP);
        // entityId = _entitySystem->createPlayerEntity(player);
        // _serverNetworkSystem->bindSessionToEntity(player->getId(), entityId);
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

        const bool success = _roomSystem->joinRoom(player, payload.roomId);
        if (!success)
            return;

        auto room = _roomSystem->getRoom(payload.roomId);
        if (!room)
            return;
        if (room->getState() != Room::State::InGame)
            return;

        rtp::net::Packet startPacket(rtp::net::OpCode::StartGame);
        _networkManager.sendPacket(sessionId, startPacket, rtp::net::NetworkMode::TCP);
        sendRoomEntitySpawnsToSession(payload.roomId, sessionId);
        // entityId = _entitySystem->createPlayerEntity(player);
        // _serverNetworkSystem->bindSessionToEntity(player->getId(), entityId);
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
        std::lock_guard lock(_mutex);
        
        rtp::net::SetReadyPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("Handle Set Ready request from Session ID {}: isReady={}",
                  sessionId, payload.isReady);
        _playerSystem->getPlayer(sessionId)->setReady(payload.isReady != 0);
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

    void GameManager::sendEntitySpawnToSessions(const rtp::ecs::Entity& entity,
                                                const std::vector<uint32_t>& sessions)
    {
        if (sessions.empty())
            return;

        auto transformRes = _registry.getComponents<rtp::ecs::components::Transform>();
        auto typeRes = _registry.getComponents<rtp::ecs::components::EntityType>();
        auto netRes = _registry.getComponents<rtp::ecs::components::NetworkId>();
        if (!transformRes || !typeRes || !netRes) {
            rtp::log::error("Missing component array for EntitySpawn");
            return;
        }

        auto &transforms = transformRes->get();
        auto &types = typeRes->get();
        auto &nets = netRes->get();
        if (!transforms.has(entity) || !types.has(entity) || !nets.has(entity)) {
            rtp::log::error("Entity {} missing Transform/EntityType/NetworkId", entity.index());
            return;
        }

        const auto &transform = transforms[entity];
        const auto &type = types[entity];
        const auto &net = nets[entity];
        rtp::net::Packet packet(rtp::net::OpCode::EntitySpawn);
        rtp::net::EntitySpawnPayload payload = {
            net.id,
            static_cast<uint8_t>(type.type),
            transform.position.x,
            transform.position.y
        };
        packet << payload;
        _networkSyncSystem->sendPacketToSessions(sessions, packet, rtp::net::NetworkMode::TCP);
    }

    void GameManager::sendRoomEntitySpawnsToSession(uint32_t roomId, uint32_t sessionId)
    {
        auto view = _registry.zipView<
            rtp::ecs::components::Transform,
            rtp::ecs::components::NetworkId,
            rtp::ecs::components::EntityType,
            rtp::ecs::components::RoomId
        >();

        for (auto&& [tf, net, type, room] : view) {
            if (room.id != roomId)
                continue;

            rtp::net::Packet packet(rtp::net::OpCode::EntitySpawn);
            rtp::net::EntitySpawnPayload payload = {
                net.id,
                static_cast<uint8_t>(type.type),
                tf.position.x,
                tf.position.y
            };
            packet << payload;
            _networkSyncSystem->sendPacketToSession(sessionId, packet, rtp::net::NetworkMode::TCP);
        }
    }
}
