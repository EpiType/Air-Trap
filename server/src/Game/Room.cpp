/**
 * File   : Room.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/Room.hpp"
#include "RType/Logger.hpp"

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

    Room::Room(uint32_t id, const std::string &name, uint32_t maxPlayers, RoomType type)
        : _id(id), _name(name), _maxPlayers(maxPlayers), _state(State::Waiting), _type(type)
    {
        /* TODO : For the V1 i will set the name with the ID */
#ifdef DEBUG
        log::info("Room '{}' type '{}' (ID: {}) created with max players: {}",
                  _name, toString(_type), _id, _maxPlayers);
#endif
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
        std::lock_guard lock(_mutex);

        if (_players.size() >= _maxPlayers) {
            log::warning("Player {} cannot join Room '{}' (ID: {}): Room is full",
                        player->getUsername(), _name, _id);
            return false;
        }

        auto sid = player->getId();
        auto it = std::find_if(_players.begin(), _players.end(),
            [sid](const auto &entry) { return entry.first->getId() == sid; });

        if (it != _players.end()) {
            log::warning("Player {} already in Room '{}' (ID: {})",
                        player->getUsername(), _name, _id);
            return false;
        }

        _players.emplace_back(player, PlayerType::Player);

        log::info("Player {} joined Room '{}' (ID: {})",
                player->getUsername(), _name, _id);

        return true;
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
} // namespace rtp::server