/**
 * File   : RoomSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR

 * *
 * LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   :
 * 11/12/2025
 */

#include "Systems/RoomSystem.hpp"
#include "RType/ECS/Components/RoomId.hpp"
#include "RType/ECS/Components/Transform.hpp"

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    RoomSystem::RoomSystem(ServerNetwork &network, ecs::Registry &registry, NetworkSyncSystem& networkSync)
        : _network(network)
        , _registry(registry)
        , _networkSync(networkSync)
    {
        auto lobby = std::make_shared<Room>(_registry, _networkSync, _nextRoomId++, "Global Lobby", 9999,
                                            0, 0, Room::RoomType::Lobby, 0, 0, 0, 0);
        _rooms[lobby->getId()] = lobby;
        _lobbyId = lobby->getId();
        log::info("Default Lobby created with ID {}", lobby->getId());
    };

    void RoomSystem::update(float dt)
    {
        launchReadyRooms(dt);
        for (auto &[roomId, roomPtr] : _rooms) {
            // (void)roomId;
            roomPtr->update(0, dt);
        }
    };

    uint32_t RoomSystem::createRoom(uint32_t sessionId,
                                    const std::string &roomName,
                                    uint8_t maxPlayers, float difficulty,
                                    float speed, Room::RoomType type,
                                    uint32_t levelId, uint32_t seed,
                                    uint32_t durationMinutes)
    {
        uint32_t newId = 0;

        newId = _nextRoomId++;

        auto room = std::make_shared<Room>(_registry, _networkSync, newId, roomName, maxPlayers,
                                           difficulty, speed, type, sessionId,
                                           levelId, seed, durationMinutes);
        _rooms.emplace(newId, room);

        log::info("Room '{}' created with ID {} by session {}", roomName, newId,
                  sessionId);
        return newId;
    }

    bool RoomSystem::joinRoom(PlayerPtr player, uint32_t roomId, bool asSpectator)
    {
        std::shared_ptr<Room> room;
        log::info(
            "Player {} (Session ID {}) attempting to join Room ID {}{}",
            player->getUsername(), player->getId(), roomId,
            asSpectator ? " as spectator" : "");
        if (!leaveRoom(player)) {
            log::warning(
                " Could not leave previous room, maybe first connection ?. Player {} (Session ID {})",
                player->getId(), roomId);
        }
        {
            std::lock_guard lock(_mutex);

            auto it = _rooms.find(roomId);
            if (it == _rooms.end()) {
                log::error(
                    "Failed to join Player {} to Room ID {}. Room does " "not "
                                                                         "exist"
                                                                         ".",
                    player->getId(), roomId);
                net::Packet response(net::OpCode::JoinRoom);
                response << static_cast<uint8_t>(0);
                _network.sendPacket(player->getId(), response, net::NetworkMode::TCP);
                return false;
            }

            room = it->second;

            if (room->isBanned(player->getUsername())) {
                log::error(
                    "Failed to join Player {} to Room ID {}. Player is banned.",
                    player->getId(), roomId);
                if (room->getType() != Room::RoomType::Lobby) {
                    net::Packet response(net::OpCode::JoinRoom);
                    response << static_cast<uint8_t>(0);
                    _network.sendPacket(player->getId(), response, net::NetworkMode::TCP);
                }
                return false;
            }

            if (!asSpectator && room->getState() != Room::State::Waiting) {
                log::error(
                    "Failed to join Player {} to Room ID {}. Game already started.",
                    player->getId(), roomId);
                if (room->getType() != Room::RoomType::Lobby) {
                    net::Packet response(net::OpCode::JoinRoom);
                    response << static_cast<uint8_t>(0);
                    _network.sendPacket(player->getId(), response, net::NetworkMode::TCP);
                }
                return false;
            }

            if (!asSpectator && !room->canJoin()) {
                log::error(
                    "Failed to join Player {} to Room ID {}. Room is full.",
                    player->getId(), roomId);
                if (room->getType() != Room::RoomType::Lobby) {
                    net::Packet response(net::OpCode::JoinRoom);
                    response << static_cast<uint8_t>(0);
                    _network.sendPacket(player->getId(), response, net::NetworkMode::TCP);
                }
                return false;
            }

            const Room::PlayerType joinType = asSpectator
                ? Room::PlayerType::Spectator
                : Room::PlayerType::Player;
            if (!room->addPlayer(player, joinType)) {
                log::error(
                    "Failed to join Player {} to Room ID {}. Could not add to new room.",
                    player->getId(), roomId);
                return false;
            }

            _playerRoomMap[player->getId()] = room->getId();
            player->setRoomId(room->getId());
            if (room->getState() == Room::State::InGame || asSpectator) {
                player->setReady(true);
                player->setState(PlayerState::InGame);
            } else {
                player->setReady(false);
                player->setState(PlayerState::InLobby);
            }

            log::info("Player {} (Session ID {}) joined Room ID {}",
                      player->getUsername(), player->getId(), room->getId());
        }
        if (room->getType() != Room::RoomType::Lobby) {
            net::Packet response(net::OpCode::JoinRoom);
            response << static_cast<uint8_t>(1);
            _network.sendPacket(player->getId(), response, net::NetworkMode::TCP);
        }
        return true;
    }

    bool RoomSystem::joinLobby(PlayerPtr player)
    {
        return joinRoom(player, _lobbyId, false);
    }

    bool RoomSystem::leaveRoom(PlayerPtr player)
    {
        if (!player) {
            return false;
        }

        std::shared_ptr<Room> previousRoom;
        {
            std::lock_guard lock(_mutex);
            auto mapIt = _playerRoomMap.find(player->getId());
            if (mapIt == _playerRoomMap.end()) {
                return false;
            }

            uint32_t previousRoomId = mapIt->second;
            auto prevIt = _rooms.find(previousRoomId);
            if (prevIt != _rooms.end()) {
                previousRoom = prevIt->second;
                prevIt->second->removePlayer(player->getId(), false);
                if (prevIt->second->getType() != Room::RoomType::Lobby &&
                    prevIt->second->getCurrentPlayerCount() == 0) {
                    _rooms.erase(prevIt);
                }
            }

            _playerRoomMap.erase(mapIt);
        }

        despawnPlayerEntity(player, previousRoom);
        return true;
    }

    void RoomSystem::disconnectPlayer(uint32_t sessionId)
    {
        std::shared_ptr<Room> room;
        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            auto it = _playerRoomMap.find(sessionId);
            if (it == _playerRoomMap.end()) {
                return;
            }

            uint32_t roomId = it->second;
            auto roomIt = _rooms.find(roomId);
            if (roomIt != _rooms.end()) {
                room = roomIt->second;
                if (room) {
                    for (const auto& roomPlayer : room->getPlayers()) {
                        if (roomPlayer && roomPlayer->getId() == sessionId) {
                            player = roomPlayer;
                            break;
                        }
                    }
                }
                roomIt->second->removePlayer(sessionId, true);
                if (roomIt->second->getType() != Room::RoomType::Lobby &&
                    roomIt->second->getCurrentPlayerCount() == 0) {
                    _rooms.erase(roomIt);
                }
            }

            _playerRoomMap.erase(it);
        }

        despawnPlayerEntity(player, room);
    }

    void RoomSystem::listAllRooms(uint32_t sessionId)
    {
        std::lock_guard lock(_mutex);
        log::info("Handle List Rooms request from Session ID {}", sessionId);

        net::Packet responsePacket(net::OpCode::RoomList);

        uint32_t roomCount = 0;
        for (const auto &[roomId, roomPtr] : _rooms) {
            (void)roomId;
            auto roomType = roomPtr->getType();
            log::info("Room ID {}: Type={} (Lobby={}, Public={}, Private={})", 
                      roomId, static_cast<int>(roomType),
                      static_cast<int>(Room::RoomType::Lobby),
                      static_cast<int>(Room::RoomType::Public),
                      static_cast<int>(Room::RoomType::Private));
            if (roomType == Room::RoomType::Lobby)
                continue;
            if (roomType == Room::RoomType::Private)
                continue;
            ++roomCount;
        }

        responsePacket << roomCount;

        for (const auto &[roomId, roomPtr] : _rooms) {
            (void)roomId;

            if (roomPtr->getType() == Room::RoomType::Lobby)
                continue;
            if (roomPtr->getType() == Room::RoomType::Private)
                continue;

            net::RoomInfo roomInfo{};
            roomInfo.roomId = roomPtr->getId();
            std::strncpy(roomInfo.roomName, roomPtr->getName().c_str(),
                         sizeof(roomInfo.roomName) - 1);
            roomInfo.roomName[sizeof(roomInfo.roomName) - 1] = '\0';

            roomInfo.currentPlayers = roomPtr->getCurrentPlayerCount();
            roomInfo.maxPlayers = roomPtr->getMaxPlayers();
            roomInfo.difficulty = roomPtr->getDifficulty();
            roomInfo.speed = roomPtr->getSpeed();
            roomInfo.inGame = (roomPtr->getState() == Room::State::InGame) ? 1 : 0;
            roomInfo.duration = roomPtr->getDurationMinutes();
            roomInfo.seed = roomPtr->getSeed();
            roomInfo.levelId = roomPtr->getLevelId();
            roomInfo.roomType = static_cast<uint8_t>(roomPtr->getType());

            responsePacket << roomInfo;

            log::info("Listed Room ID {}: Name='{}', Players={}/{}",
                           roomInfo.roomId, roomInfo.roomName,
                           roomInfo.currentPlayers, roomInfo.maxPlayers);
        }

        _network.sendPacket(sessionId, responsePacket,
                            net::NetworkMode::TCP);
        log::info("Sent Room List ({} rooms) to Session ID {}", roomCount,
                       sessionId);
    }

    void RoomSystem::chatInRoom(uint32_t sessionId,
                                const net::Packet &packet)
    {
        // Implementation for chatting in a room
    }

    void RoomSystem::launchReadyRooms(float dt)
    {
        std::vector<std::shared_ptr<Room>> toStart;

        {
            std::lock_guard lock(_mutex);

            for (auto &[roomId, roomPtr] : _rooms) {
                if (roomPtr->getType() == Room::RoomType::Lobby)
                    continue;

                if (roomPtr->getState() != Room::State::Waiting)
                    continue;

                bool allReady = true;
                for (const auto &player : roomPtr->getPlayers()) {
                    if (!player->isReady()) {
                        allReady = false;
                        break;
                    }
                }

                if (allReady && roomPtr->getCurrentPlayerCount() > 0) {
                    toStart.push_back(roomPtr);
                }
            }
        }

        for (auto &roomPtr : toStart) {
            const bool started = roomPtr->startGame(dt);

            if (started && _onRoomStarted) {
                _onRoomStarted(roomPtr->getId());
            }

            if (roomPtr->getState() == Room::State::InGame) {
                log::info("Launching Room ID {} as all players are ready.",
                               roomPtr->getId());

                net::Packet startPacket(net::OpCode::StartGame);
                for (const auto &player : roomPtr->getPlayers()) {
                    log::info(
                        "Notifying Player {} (Session ID {}) of game start "
                        "in Room ID {}.",
                        player->getUsername(), player->getId(),
                        roomPtr->getId());
                    _network.sendPacket(player->getId(), startPacket,
                                        net::NetworkMode::TCP);
                }
            } else {
                log::debug(
                    "Room ID {} did NOT start (canStartGame/state blocked).",
                    roomPtr->getId());
            }
        }
    }

    std::shared_ptr<Room> RoomSystem::getRoom(uint32_t roomId)
    {
        std::lock_guard lock(_mutex);

        auto it = _rooms.find(roomId);
        if (it != _rooms.end()) {
            return it->second;
        }
        return nullptr;
    }

    void RoomSystem::despawnPlayerEntity(const PlayerPtr& player,
                                         const std::shared_ptr<Room>& room)
    {
        if (!player) {
            return;
        }

        const uint32_t entityId = player->getEntityId();
        if (entityId == 0) {
            _networkSync.unbindSession(player->getId());
            return;
        }

        ecs::Entity entity(entityId, 0);
        auto transformsRes = _registry.get<ecs::components::Transform>();
        auto typesRes = _registry.get<ecs::components::EntityType>();
        auto netsRes = _registry.get<ecs::components::NetworkId>();
        auto roomsRes = _registry.get<ecs::components::RoomId>();

        if (transformsRes && typesRes && netsRes && roomsRes) {
            auto &transforms = transformsRes->get();
            auto &types = typesRes->get();
            auto &nets = netsRes->get();
            auto &rooms = roomsRes->get();
            if (transforms.has(entity) && types.has(entity) && nets.has(entity) && rooms.has(entity)) {
                const bool shouldBroadcast = room && room->getType() != Room::RoomType::Lobby;
                if (shouldBroadcast) {
                    const auto players = room->getPlayers();
                    if (!players.empty()) {
                        net::Packet packet(net::OpCode::EntityDeath);
                        net::EntityDeathPayload payload{};
                        payload.netId = nets[entity].id;
                        payload.type = static_cast<uint8_t>(types[entity].type);
                        payload.position = transforms[entity].position;
                        packet << payload;

                        for (const auto& roomPlayer : players) {
                            _networkSync.sendPacketToSession(
                                roomPlayer->getId(), packet, net::NetworkMode::TCP);
                        }
                    }
                }
            }
        }

        _registry.kill(entity);
        _networkSync.unbindSession(player->getId());
        player->setEntityId(0);
    }
}
