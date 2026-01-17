/**
 * File   : RoomSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/RoomSystem.hpp"

#include "engine/core/Logger.hpp"

#include <cstring>

namespace rtp::server::systems
{

    /////////////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////////////

    RoomSystem::RoomSystem(engine::net::INetwork &network, PlayerSystem &players)
        : _network(network),
          _players(players)
    {
        auto lobby = std::make_shared<Room>(_nextRoomId++,
                                            "Global Lobby",
                                            9999,
                                            0.0f,
                                            0.0f,
                                            0,
                                            0,
                                            0);
        lobby->setType(Room::RoomType::Lobby);
        _lobbyId = lobby->id();
        _rooms.emplace(_lobbyId, lobby);
    }

    uint32_t RoomSystem::createRoom(uint32_t sessionId,
                                    const std::string &roomName,
                                    uint8_t maxPlayers,
                                    float difficulty,
                                    float speed,
                                    Room::RoomType type,
                                    uint32_t levelId,
                                    uint32_t seed,
                                    uint32_t duration)
    {
        const uint32_t newId = _nextRoomId++;
        auto room = std::make_shared<Room>(newId,
                                           roomName,
                                           maxPlayers,
                                           difficulty,
                                           speed,
                                           duration,
                                           seed,
                                           levelId);
        room->setType(type);
        room->setOwner(sessionId);
        _rooms.emplace(newId, room);
        return newId;
    }

    bool RoomSystem::joinRoom(const PlayerPtr &player, uint32_t roomId, bool asSpectator)
    {
        if (!player) {
            return false;
        }

        if (!leaveRoom(player)) {
            engine::core::warning("Could not leave previous room for session {}", player->id());
        }

        std::shared_ptr<Room> room;
        {
            std::lock_guard lock(_mutex);
            auto it = _rooms.find(roomId);
            if (it == _rooms.end()) {
                rtp::net::Packet response(rtp::net::OpCode::JoinRoom);
                response << rtp::net::BooleanPayload{0};
                const auto data = response.serialize();
                _network.sendPacket(player->id(), data, engine::net::NetChannel::TCP);
                return false;
            }

            room = it->second;

            if (room->isBanned(player->username())) {
                if (room->type() != Room::RoomType::Lobby) {
                    rtp::net::Packet response(rtp::net::OpCode::JoinRoom);
                    response << rtp::net::BooleanPayload{0};
                    const auto data = response.serialize();
                    _network.sendPacket(player->id(), data, engine::net::NetChannel::TCP);
                }
                return false;
            }

            if (!asSpectator && room->state() != Room::State::Waiting) {
                if (room->type() != Room::RoomType::Lobby) {
                    rtp::net::Packet response(rtp::net::OpCode::JoinRoom);
                    response << rtp::net::BooleanPayload{0};
                    const auto data = response.serialize();
                    _network.sendPacket(player->id(), data, engine::net::NetChannel::TCP);
                }
                return false;
            }

            if (!asSpectator && !room->canJoin()) {
                if (room->type() != Room::RoomType::Lobby) {
                    rtp::net::Packet response(rtp::net::OpCode::JoinRoom);
                    response << rtp::net::BooleanPayload{0};
                    const auto data = response.serialize();
                    _network.sendPacket(player->id(), data, engine::net::NetChannel::TCP);
                }
                return false;
            }

            if (!room->addPlayer(player->id())) {
                return false;
            }

            _playerRoomMap[player->id()] = room->id();
            player->setRoomId(room->id());
            if (room->state() == Room::State::InGame || asSpectator) {
                player->setReady(true);
            } else {
                player->setReady(false);
            }
        }

        if (room->type() != Room::RoomType::Lobby) {
            rtp::net::Packet response(rtp::net::OpCode::JoinRoom);
            response << rtp::net::BooleanPayload{1};
            const auto data = response.serialize();
            _network.sendPacket(player->id(), data, engine::net::NetChannel::TCP);
        }
        return true;
    }

    bool RoomSystem::joinLobby(const PlayerPtr &player)
    {
        return joinRoom(player, _lobbyId, false);
    }

    bool RoomSystem::leaveRoom(const PlayerPtr &player)
    {
        if (!player) {
            return false;
        }

        std::shared_ptr<Room> previousRoom;
        {
            std::lock_guard lock(_mutex);
            auto mapIt = _playerRoomMap.find(player->id());
            if (mapIt == _playerRoomMap.end()) {
                return false;
            }

            const uint32_t previousRoomId = mapIt->second;
            auto roomIt = _rooms.find(previousRoomId);
            if (roomIt != _rooms.end()) {
                previousRoom = roomIt->second;
                if (!previousRoom->removePlayer(player->id())) {
                    engine::core::warning("Room {}: failed to remove session {}",
                                          previousRoom->id(),
                                          player->id());
                }
                if (previousRoom->type() != Room::RoomType::Lobby &&
                    previousRoom->playerCount() == 0) {
                    _rooms.erase(roomIt);
                }
            }

            _playerRoomMap.erase(mapIt);
        }

        player->setRoomId(0);
        player->setReady(false);
        return true;
    }

    void RoomSystem::disconnectPlayer(uint32_t sessionId)
    {
        auto player = _players.getPlayer(sessionId);
        if (!player) {
            return;
        }
        leaveRoom(player);
    }

    void RoomSystem::listAllRooms(uint32_t sessionId)
    {
        std::lock_guard lock(_mutex);

        std::vector<rtp::net::RoomInfo> infos;
        for (const auto &[roomId, roomPtr] : _rooms) {
            (void)roomId;
            if (roomPtr->type() == Room::RoomType::Lobby) {
                continue;
            }
            infos.push_back(roomPtr->toInfo());
        }

        rtp::net::Packet response(rtp::net::OpCode::RoomList);
        response << infos;
        const auto data = response.serialize();
        _network.sendPacket(sessionId, data, engine::net::NetChannel::TCP);
    }

    void RoomSystem::launchReadyRooms(float)
    {
        std::vector<RoomPtr> toStart;
        {
            std::lock_guard lock(_mutex);
            for (const auto &[id, room] : _rooms) {
                (void)id;
                if (room->type() == Room::RoomType::Lobby) {
                    continue;
                }
                if (room->state() != Room::State::Waiting) {
                    continue;
                }

                bool allReady = true;
                for (const auto sessionId : room->players()) {
                    auto player = _players.getPlayer(sessionId);
                    if (!player || !player->ready()) {
                        allReady = false;
                        break;
                    }
                }

                if (allReady && room->playerCount() > 0) {
                    toStart.push_back(room);
                }
            }
        }

        for (auto &room : toStart) {
            room->setState(Room::State::InGame);

            rtp::net::Packet start(rtp::net::OpCode::StartGame);
            const auto data = start.serialize();
            for (const auto sessionId : room->players()) {
                _network.sendPacket(sessionId, data, engine::net::NetChannel::TCP);
            }

            if (_onRoomStarted) {
                _onRoomStarted(room->id());
            }
        }
    }

    RoomSystem::RoomPtr RoomSystem::getRoom(uint32_t roomId)
    {
        std::lock_guard lock(_mutex);
        auto it = _rooms.find(roomId);
        if (it == _rooms.end()) {
            return nullptr;
        }
        return it->second;
    }

    std::vector<RoomSystem::RoomPtr> RoomSystem::rooms(void) const
    {
        std::lock_guard lock(_mutex);
        std::vector<RoomPtr> result;
        result.reserve(_rooms.size());
        for (const auto &[id, room] : _rooms) {
            (void)id;
            result.push_back(room);
        }
        return result;
    }

    uint32_t RoomSystem::lobbyId(void) const
    {
        return _lobbyId;
    }

    void RoomSystem::setOnRoomStarted(OnRoomStarted cb)
    {
        _onRoomStarted = std::move(cb);
    }
}
