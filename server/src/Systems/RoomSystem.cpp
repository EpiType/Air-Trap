/**
 * File   : RoomSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR
 * LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/RoomSystem.hpp"

using namespace rtp::net;

namespace rtp::server
{

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    RoomSystem::RoomSystem(ServerNetwork &network, rtp::ecs::Registry &registry)
        : _network(network)
        , _registry(registry)
    {
        auto lobby = std::make_shared<Room>(_nextRoomId++, "Global Lobby", 9999,
                                            0, 0, Room::RoomType::Lobby);
        _rooms[lobby->getId()] = lobby;
        _lobbyId = lobby->getId();
        log::info("Default Lobby created with ID {}", lobby->getId());
    };

    void RoomSystem::update(float dt)
    {
        (void)dt;
    };

    uint32_t RoomSystem::createRoom(uint32_t sessionId, const std::string &roomName, uint8_t maxPlayers,
                                float difficulty, float speed, Room::RoomType type)
    {
        uint32_t newId = 0;

        newId = _nextRoomId++;

        auto room = std::make_shared<Room>(
            newId, roomName, maxPlayers, difficulty, speed, type, sessionId
        );
        _rooms.emplace(newId, room);

        log::info("Room '{}' created with ID {} by session {}", roomName, newId, sessionId);
        return newId;
    }

    bool RoomSystem::joinRoom(PlayerPtr player, uint32_t roomId)
    {
        std::shared_ptr<Room> room;
        rtp::log::info("Player {} (Session ID {}) attempting to join Room ID {}",
                player->getUsername(), player->getId(), roomId);
        {
            std::lock_guard lock(_mutex);

            auto it = _rooms.find(roomId);
            if (it == _rooms.end()) {
                log::error("Failed to join Player {} to Room ID {}. Room does not exist.",
                        player->getId(), roomId);
                return false;
            }

            room = it->second;

            if (!room->canJoin()) {
                log::error("Failed to join Player {} to Room ID {}. Room is full.",
                        player->getId(), roomId);
                return false;
            }

            if (!leaveRoom(player)) {
                log::warning(" Could not leave previous room, maybe first connection ?. Player {} (Session ID {})",
                        player->getId(), roomId);
            }

            if (!room->addPlayer(player)) {
                log::error("Failed to join Player {} to Room ID {}. Could not add to new room.",
                        player->getId(), roomId);
                return false;
            }

            _playerRoomMap[player->getId()] = room->getId();
            player->setRoomId(room->getId());
            player->setState(PlayerState::InLobby);

            log::info("Player {} (Session ID {}) joined Room ID {}",
                player->getUsername(), player->getId(), room->getId());
        }
        return true;
    }

    bool RoomSystem::joinLobby(PlayerPtr player)
    {
        return joinRoom(player, _lobbyId);
    }

    bool RoomSystem::leaveRoom(PlayerPtr player)
    {
        if (_playerRoomMap.find(player->getId()) != _playerRoomMap.end()) {
            try {
                uint32_t previousRoomId = _playerRoomMap[player->getId()];
                auto prevIt = _rooms.find(previousRoomId);
                if (prevIt != _rooms.end()) {
                    prevIt->second->removePlayer(player->getId());
                    // sendRoomUpdate(*prevIt->second);
                    return true;
                }
            } catch (const std::exception &e) {
                rtp::log::error("Exception while removing Player {} from previous room: {}", player->getId(), e.what());
                return false;
            }
        }
        return false;
    }

    void RoomSystem::disconnectPlayer(uint32_t sessionId)
    {
        auto it = _playerRoomMap.find(sessionId);

        if (it != _playerRoomMap.end()) {
            uint32_t roomId = it->second;
            auto roomIt = _rooms.find(roomId);
            if (roomIt != _rooms.end()) {
                roomIt->second->removePlayer(sessionId);
                // sendRoomUpdate(*roomIt->second);
            }
            _playerRoomMap.erase(it);
        }
    }

    void RoomSystem::listAllRooms(uint32_t sessionId, const rtp::net::Packet &packet)
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
            rtp::log::info("Listed Room ID {}: Name='{}', Players={}/{}",
                      roomInfo.roomId, roomInfo.roomName,
                      roomInfo.currentPlayers, roomInfo.maxPlayers);
        }

        _network.sendPacket(sessionId, responsePacket, rtp::net::NetworkMode::TCP);
        rtp::log::info("Sent Room List to Session ID {}", sessionId);
    }

    void RoomSystem::chatInRoom(uint32_t sessionId, const rtp::net::Packet &packet)
    {
        // Implementation for chatting in a room
    }
} // namespace rtp::server
