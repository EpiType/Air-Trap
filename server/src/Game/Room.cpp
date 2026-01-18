/**
 * File   : Room.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/Room.hpp"
#include "RType/Logger.hpp"
#include "RType/ECS/Components/RoomId.hpp"
#include "RType/ECS/Components/Velocity.hpp"

#include <cstring>

using namespace rtp::ecs;

#ifdef DEBUG
    #include <iostream>
static const char *toString(rtp::server::Room::RoomType t)
{
    switch (t) {
        case rtp::server::Room::RoomType::Lobby:   return "Lobby";
        case rtp::server::Room::RoomType::Public:  return "Public";
        case rtp::server::Room::RoomType::Private: return "Private";
        default: return "Unknown";
    }
}

static const char *toString(rtp::server::Room::PlayerType t)
{
    switch (t) {
        case rtp::server::Room::PlayerType::Player:    return "Player";
        case rtp::server::Room::PlayerType::Spectator: return "Spectator";
        default: return "Unknown";
    }
}
#endif

namespace rtp::server
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    Room::Room(Registry& registry, NetworkSyncSystem& network, uint32_t id, const std::string &name,
               uint32_t maxPlayers, float difficulty, float speed, RoomType type,
               uint32_t creatorSessionId, uint32_t levelId, uint32_t seed,
               uint32_t durationMinutes)
        : _registry(registry)
        , _network(network)
        , _id(id)
        , _name(name)
        , _maxPlayers(maxPlayers)
        , _state(State::Waiting)
        , _type(type)
        , _creatorSessionId(creatorSessionId)
        , _levelId(levelId)
        , _seed(seed)
        , _difficulty(difficulty)
        , _speed(speed)
        , durationMinutes(durationMinutes)
    {
        log::info("Room '{}' (ID: {}) created with max players: {} by session {}",
                  _name, _id, _maxPlayers, creatorSessionId);
    }

    Room::~Room()
    {
        log::info("Room '{}' (ID: {}) destroyed", _name, _id);
    }

    bool Room::canJoin() const
    {
        std::lock_guard lock(_mutex);
        return _players.size() < _maxPlayers;
    }

    bool Room::addPlayer(const PlayerPtr &player, PlayerType type)
    {
        const uint32_t sessionId = player->getId();
        bool added = false;
        std::string username;
        {
            std::lock_guard lock(_mutex);

            if (_bannedUsers.find(player->getUsername()) != _bannedUsers.end()) {
                log::warning("Player {} cannot join Room '{}' (ID: {}): banned",
                            player->getUsername(), _name, _id);
            } else if (type == PlayerType::Player && _players.size() >= _maxPlayers) {
                log::warning("Player {} cannot join Room '{}' (ID: {}): Room is full",
                            player->getUsername(), _name, _id);
            } else if (type == PlayerType::Player && _state != State::Waiting) {
                log::warning("Player {} cannot join Room '{}' (ID: {}): Game already started",
                            player->getUsername(), _name, _id);
            } else {
                auto it = std::find_if(_players.begin(), _players.end(),
                    [sessionId](const auto &entry) { return entry.first->getId() == sessionId; });

                if (it != _players.end()) {
                    log::warning("Player {} already in Room '{}' (ID: {})",
                                player->getUsername(), _name, _id);
                } else {
                    _players.emplace_back(player, type);
                    log::info("Player {} joined Room '{}' (ID: {})",
                            player->getUsername(), _name, _id);
                    username = player->getUsername();
                    added = true;
                }
            }
        }

        if (_type != RoomType::Lobby) {
            net::Packet response(net::OpCode::JoinRoom);
            const uint8_t success = added ? 1 : 0;
            response << success;
            _network.sendPacketToSession(sessionId, response, net::NetworkMode::TCP);
        }

        if (added && _type != RoomType::Lobby) {
            if (type == PlayerType::Spectator) {
                broadcastSystemMessage(username + " (spectator) has joined");
            } else {
                broadcastSystemMessage(username + " has joined the room");
            }
        }

        return added;
    }

    void Room::removePlayer(uint32_t sessionId, bool disconnected)
    {
        bool removed = false;
        std::string username;
        PlayerType type = PlayerType::None;
        {
            std::lock_guard lock(_mutex);

            auto before = _players.size();
            for (const auto &entry : _players) {
                if (entry.first->getId() == sessionId) {
                    username = entry.first->getUsername();
                    type = entry.second;
                    break;
                }
            }
            _players.remove_if([sessionId](const auto &entry) {
                return entry.first->getId() == sessionId;
            });

            if (_players.size() != before) {
                removed = true;
                log::info("Player with Session ID {} removed from Room '{}' (ID: {})",
                        sessionId, _name, _id);
            } else {
                log::warning("Player with Session ID {} not found in Room '{}' (ID: {})",
                            sessionId, _name, _id);
            }
        }

        if (removed && _type != RoomType::Lobby) {
            const std::string suffix = (type == PlayerType::Spectator) ? " (spectator)" : "";
            if (disconnected) {
                broadcastSystemMessage(username + suffix + " has disconnected");
            } else {
                broadcastSystemMessage(username + suffix + " has left the room");
            }
        }
    }

    void Room::broadcastSystemMessage(const std::string &message)
    {
        std::vector<uint32_t> sessions;
        {
            std::lock_guard lock(_mutex);
            sessions.reserve(_players.size());
            for (const auto &entry : _players) {
                sessions.push_back(entry.first->getId());
            }
        }

        net::RoomChatReceivedPayload payload{};
        payload.sessionId = 0;
        payload.username[0] = '\0';
        std::strncpy(payload.message, message.c_str(), sizeof(payload.message) - 1);
        payload.message[sizeof(payload.message) - 1] = '\0';

        net::Packet packet(net::OpCode::RoomChatReceived);
        packet << payload;

        for (uint32_t sid : sessions) {
            _network.sendPacketToSession(sid, packet, net::NetworkMode::TCP);
        }
    }

    void Room::banUser(const std::string &username)
    {
        std::lock_guard lock(_mutex);
        _bannedUsers.insert(username);
    }

    bool Room::isBanned(const std::string &username) const
    {
        std::lock_guard lock(_mutex);
        return _bannedUsers.find(username) != _bannedUsers.end();
    }

    const std::list<PlayerPtr> Room::getPlayers(void) const
    {
        std::lock_guard lock(_mutex);

        std::list<PlayerPtr> out;
        out.resize(0);
        for (const auto &entry : _players) {
            out.push_back(entry.first);
        }
        return out;
    }

    uint32_t Room::getId(void) const
    {
        return _id;
    }

    Room::RoomType Room::getType() const
    {
        return _type;
    }

    float Room::getDifficulty() const
    {
        return _difficulty;
    }

    float Room::getSpeed() const
    {
        return _speed;
    }

    uint32_t Room::getLevelId() const
    {
        return _levelId;
    }

    uint32_t Room::getSeed() const
    {
        return _seed;
    }

    uint32_t Room::getDurationMinutes() const
    {
        return durationMinutes;
    }

    Room::PlayerType Room::getPlayerType(uint32_t sessionId) const
    {
        std::lock_guard lock(_mutex);

        for (const auto &entry : _players) {
            if (entry.first->getId() == sessionId) {
                return entry.second;
            }
        }

        log::warning("Player with Session ID {} not found in Room '{}' (ID: {})",
                    sessionId, _name, _id);
        return PlayerType::None;
    }

    void Room::setPlayerType(uint32_t sessionId, PlayerType type)
    {
        std::lock_guard lock(_mutex);

        for (auto &entry : _players) {
            if (entry.first->getId() == sessionId) {
                entry.second = type;
#ifdef DEBUG
                log::info("Player with Session ID {} set to type {} in Room '{}' (ID: {})",
                        sessionId, toString(type), _name, _id);
#endif
                return;
            }
        }

        log::warning("Player with Session ID {} not found in Room '{}' (ID: {})",
                    sessionId, _name, _id);
    }

    std::string Room::getName(void) const
    {
        return _name;
    }

    uint32_t Room::getMaxPlayers(void) const
    {
        return _maxPlayers;
    }

    uint32_t Room::getCurrentPlayerCount(void) const
    {
        std::lock_guard lock(_mutex);
        return static_cast<uint32_t>(_players.size());
    }

    Room::State Room::getState() const
    {
        std::lock_guard lock(_mutex);
        return _state;
    }

    bool Room::startGame(float dt)
    {
        std::lock_guard lock(_mutex);
        _startedDt += dt;

        if (_state != State::Waiting) {
            log::warning("Room '{}' (ID: {}) cannot start game: Invalid state",
                        _name, _id);
            return false;
        }

        _state = State::InGame;
        log::info("Game started in Room '{}' (ID: {})", _name, _id);
        return true;
    }

    void Room::forceFinishGame(void)
    {
        std::lock_guard lock(_mutex);
        if (_state != State::InGame) {
            log::warning("Room '{}' (ID: {}) cannot finish game: Invalid state",
                        _name, _id);
            return;
        }

        _state = State::Finished;
        log::info("Game finished in Room '{}' (ID: {})", _name, _id);
    }

    void Room::update(uint32_t serverTick, float dt)
    {
        (void)dt;
        broadcastRoomState(serverTick);
    }

    void Room::broadcastRoomState(uint32_t serverTick)
    {
        std::vector<uint32_t> sessions;
        {
            std::lock_guard lock(_mutex);
            if (_type == RoomType::Lobby)
                return;
            if (_state != State::InGame)
                return;

            sessions.reserve(_players.size());
            for (const auto& entry : _players) {
                sessions.push_back(entry.first->getId());
            }
        }

        std::vector<net::EntitySnapshotPayload> snapshots;

        auto transformsRes = _registry.get<ecs::components::Transform>();
        auto networkIdsRes = _registry.get<ecs::components::NetworkId>();
        auto roomIdsRes = _registry.get<ecs::components::RoomId>();
        auto velocitiesRes = _registry.get<ecs::components::Velocity>();
        
        if (!transformsRes || !networkIdsRes || !roomIdsRes)
            return;
        
        auto& transforms = transformsRes->get();
        auto& networkIds = networkIdsRes->get();
        auto& roomIds = roomIdsRes->get();

        for (auto entity : transforms.entities()) {
            if (!networkIds.has(entity) || !roomIds.has(entity))
                continue;
            if (roomIds[entity].id != _id)
                continue;
            
            Vec2f velocity{0.0f, 0.0f};
            if (velocitiesRes) {
                auto& velocities = velocitiesRes->get();
                if (velocities.has(entity)) {
                    const auto& vel = velocities[entity];
                    velocity = vel.direction * vel.speed;
                }
            }
            
            snapshots.push_back({
                networkIds[entity].id,
                transforms[entity].position,
                velocity,
                transforms[entity].rotation
            });
        }

        // log::debug("Room '{}' (ID: {}) broadcasting {} entity snapshots",
        //                _name, _id, snapshots.size());
        if (snapshots.empty())
            return;

        net::Packet packet(net::OpCode::RoomUpdate);
        net::RoomSnapshotPayload header = {
            serverTick,
            static_cast<uint16_t>(snapshots.size())
        };
        packet << header << snapshots;

        for (uint32_t sid : sessions) {
            _network.sendPacketToSession(sid, packet, net::NetworkMode::UDP);
        }
    }
} // namespace rtp::server
