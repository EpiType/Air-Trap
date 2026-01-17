/**
 * File   : Room.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/game/Room.hpp"

#include <algorithm>
#include <cstring>

namespace rtp::server
{

    /////////////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////////////

    Room::Room(uint32_t id,
               std::string name,
               uint32_t maxPlayers,
               float difficulty,
               float speed,
               uint32_t duration,
               uint32_t seed,
               uint32_t levelId)
        : _id(id),
          _name(std::move(name)),
          _maxPlayers(maxPlayers),
          _difficulty(difficulty),
          _speed(speed),
          _duration(duration),
          _seed(seed),
          _levelId(levelId)
    {
    }

    uint32_t Room::id(void) const
    {
        return _id;
    }

    const std::string &Room::name(void) const
    {
        return _name;
    }

    uint32_t Room::maxPlayers(void) const
    {
        return _maxPlayers;
    }

    bool Room::inGame(void) const
    {
        return _state == State::InGame;
    }

    void Room::setInGame(bool inGame)
    {
        _state = inGame ? State::InGame : State::Waiting;
    }

    Room::State Room::state(void) const
    {
        return _state;
    }

    void Room::setState(State state)
    {
        _state = state;
    }

    Room::RoomType Room::type(void) const
    {
        return _type;
    }

    void Room::setType(RoomType type)
    {
        _type = type;
    }

    void Room::setOwner(uint32_t sessionId)
    {
        _owner = sessionId;
    }

    uint32_t Room::owner(void) const
    {
        return _owner;
    }

    bool Room::canJoin(void) const
    {
        return _players.size() < _maxPlayers && _state == State::Waiting;
    }

    bool Room::addPlayer(uint32_t sessionId)
    {
        if (hasPlayer(sessionId) || !canJoin()) {
            return false;
        }
        _players.push_back(sessionId);
        return true;
    }

    bool Room::removePlayer(uint32_t sessionId)
    {
        auto it = std::find(_players.begin(), _players.end(), sessionId);
        if (it == _players.end()) {
            return false;
        }
        _players.erase(it);
        return true;
    }

    bool Room::hasPlayer(uint32_t sessionId) const
    {
        return std::find(_players.begin(), _players.end(), sessionId) != _players.end();
    }

    std::size_t Room::playerCount(void) const
    {
        return _players.size();
    }

    const std::vector<uint32_t> &Room::players(void) const
    {
        return _players;
    }

    bool Room::isBanned(const std::string &username) const
    {
        return _banned.contains(username);
    }

    void Room::ban(const std::string &username)
    {
        _banned.insert(username);
    }

    void Room::unban(const std::string &username)
    {
        _banned.erase(username);
    }

    rtp::net::RoomInfo Room::toInfo(void) const
    {
        rtp::net::RoomInfo info{};
        info.roomId = _id;
        std::memset(info.roomName, 0, sizeof(info.roomName));
        std::strncpy(info.roomName, _name.c_str(), sizeof(info.roomName) - 1);
        info.currentPlayers = static_cast<uint32_t>(_players.size());
        info.maxPlayers = _maxPlayers;
        info.inGame = _state == State::InGame ? 1 : 0;
        info.difficulty = _difficulty;
        info.speed = _speed;
        info.duration = _duration;
        info.seed = _seed;
        info.levelId = _levelId;
        return info;
    }

    float Room::speed(void) const
    {
        return _speed;
    }
}
