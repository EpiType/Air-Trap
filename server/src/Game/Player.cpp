/**
 * File   : Player.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/Player.hpp"

namespace rtp::server
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    Player::Player(uint32_t sessionId, const std::string &username)
        : _sessionId(sessionId), _state(PlayerState::Connected), _entityId(0)
    {
        if (!username.empty()) {
            /* TODO : For the V1 i will set the username with the ID */
            _username = "Player_" + std::to_string(sessionId);
        } else {
            _username = "Player_" + std::to_string(sessionId);
        }
    }

    Player::~Player()
    {
    }

    void Player::setUsername(const std::string &username)
    {
        std::lock_guard lock(_mutex);
        _username = username;
    }

    std::string Player::getUsername() const
    {
        std::lock_guard lock(_mutex);
        return _username;
    }

    void Player::setRoomId(uint32_t roomId)
    {
        std::lock_guard lock(_mutex);
        _roomId = roomId;
    }

    uint32_t Player::getRoomId() const
    {
        std::lock_guard lock(_mutex);
        return _roomId;
    }

    bool Player::isInRoom() const
    {
        std::lock_guard lock(_mutex);
        return _roomId != 0;
    }

    void Player::setState(PlayerState state)
    {
        std::lock_guard lock(_mutex);
        _state = state;
    }

    PlayerState Player::getState() const
    {
        std::lock_guard lock(_mutex);
        return _state;
    }

    void Player::setReady(bool ready)
    {
        std::lock_guard lock(_mutex);
        _isReady = ready;
    }

    bool Player::isReady() const
    {
        std::lock_guard lock(_mutex);
        return _isReady;
    }

    void Player::setEntityId(uint32_t entityId)
    {
        std::lock_guard lock(_mutex);
        _entityId = entityId;
    }

    uint32_t Player::getId() const
    {
        std::lock_guard lock(_mutex);
        return _sessionId;
    }

    uint32_t Player::getEntityId() const
    {
        std::lock_guard lock(_mutex);
        return _entityId;
    }
} // namespace rtp::server