/**
 * File   : Room.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/Room.hpp"
#include "RType/Logger.hpp"
#include "RType/ECS/Components/RoomId.hpp"

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

    Room::Room(Registry& registry, NetworkSyncSystem& network, uint32_t id, const std::string &name, uint32_t maxPlayers, float difficulty, float speed, RoomType type, uint32_t creatorSessionId)
        : _registry(registry), _network(network), _id(id), _name(name), _maxPlayers(maxPlayers), _state(State::Waiting), _type(type), _difficulty(difficulty), _speed(speed), _creatorSessionId(creatorSessionId)
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

    bool Room::addPlayer(const PlayerPtr &player)
    {
        const uint32_t sessionId = player->getId();
        bool added = false;
        {
            std::lock_guard lock(_mutex);

            if (_players.size() >= _maxPlayers) {
                log::warning("Player {} cannot join Room '{}' (ID: {}): Room is full",
                            player->getUsername(), _name, _id);
            } else if (_state != State::Waiting) {
                log::warning("Player {} cannot join Room '{}' (ID: {}): Game already started",
                            player->getUsername(), _name, _id);
            } else {
                auto it = std::find_if(_players.begin(), _players.end(),
                    [sessionId](const auto &entry) { return entry.first->getId() == sessionId; });

                if (it != _players.end()) {
                    log::warning("Player {} already in Room '{}' (ID: {})",
                                player->getUsername(), _name, _id);
                } else {
                    _players.emplace_back(player, PlayerType::Player);
                    log::info("Player {} joined Room '{}' (ID: {})",
                            player->getUsername(), _name, _id);
                    added = true;
                }
            }
        }

        if (_type != RoomType::Lobby) {
            rtp::net::Packet response(rtp::net::OpCode::JoinRoom);
            const uint8_t success = added ? 1 : 0;
            response << success;
            _network.sendPacketToSession(sessionId, response, rtp::net::NetworkMode::TCP);
        }

        return added;
    }

    void Room::removePlayer(uint32_t sessionId)
    {
        std::lock_guard lock(_mutex);

        auto before = _players.size();
        _players.remove_if([sessionId](const auto &entry) {
            return entry.first->getId() == sessionId;
        });

        if (_players.size() != before) {
            log::info("Player with Session ID {} removed from Room '{}' (ID: {})",
                    sessionId, _name, _id);
        } else {
            log::warning("Player with Session ID {} not found in Room '{}' (ID: {})",
                        sessionId, _name, _id);
        }
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

        std::vector<rtp::net::EntitySnapshotPayload> snapshots;

        auto view = _registry.zipView<
            rtp::ecs::components::Transform,
            rtp::ecs::components::NetworkId,
            rtp::ecs::components::RoomId
        >();

        for (auto&& [tf, net, room] : view) {
            if (room.id != _id)
                continue;
            snapshots.push_back({
                net.id,
                tf.position,
                {0.0f, 0.0f},
                tf.rotation
            });
        }

        rtp::log::debug("Room '{}' (ID: {}) broadcasting {} entity snapshots",
                       _name, _id, snapshots.size());
        if (snapshots.empty())
            return;

        rtp::net::Packet packet(rtp::net::OpCode::RoomUpdate);
        rtp::net::RoomSnapshotPayload header = {
            serverTick,
            static_cast<uint16_t>(snapshots.size())
        };
        packet << header << snapshots;

        for (uint32_t sid : sessions) {
            _network.sendPacketToSession(sid, packet, rtp::net::NetworkMode::UDP);
        }
    }
} // namespace rtp::server
