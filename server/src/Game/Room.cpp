/**
 * File   : Room.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/Room.hpp"
#include "RType/Logger.hpp"

namespace rtp::server
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    Room::Room(uint32_t id, const std::string &name, uint32_t maxPlayers)
        : _id(id), _name(name), _maxPlayers(maxPlayers), _state(State::Waiting)
    {
        /* TODO : For the V1 i will set the name with the ID */
        log::info("Room '{}' (ID: {}) created with max players: {}",
                  _name, _id, _maxPlayers);
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

    void Room::addPlayer(const PlayerPtr &player)
    {
        std::lock_guard lock(_mutex);
        if (_players.size() < _maxPlayers) {
            _players.push_back(player);
            log::info("Player {} joined Room '{}' (ID: {})",
                      player->getUsername(), _name, _id);
        } else {
            log::warning("Player {} failed to join Room '{}' (ID: {}): Room is full",
                         player->getUsername(), _name, _id);
        }
    }

    void Room::removePlayer(uint32_t sessionId)
    {
        std::lock_guard lock(_mutex);
        _players.remove_if([sessionId](const PlayerPtr &player) {
            return player->getId() == sessionId;
        });
        log::info("Player with Session ID {} removed from Room '{}' (ID: {})",
                  sessionId, _name, _id);
    }

    const std::list<PlayerPtr>& Room::getPlayers(void) const
    {
        std::lock_guard lock(_mutex);
        return _players;
    }

    uint32_t Room::getId(void) const
    {
        return _id;
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