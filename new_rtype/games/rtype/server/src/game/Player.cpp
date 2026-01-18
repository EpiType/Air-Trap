/**
 * File   : Player.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/game/Player.hpp"

namespace rtp::server
{

    /////////////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////////////

    Player::Player(uint32_t sessionId)
        : _sessionId(sessionId)
    {
    }

    uint32_t Player::id(void) const
    {
        return _sessionId;
    }

    const std::string &Player::username(void) const
    {
        return _username;
    }

    void Player::setUsername(std::string username)
    {
        _username = std::move(username);
    }

    bool Player::loggedIn(void) const
    {
        return _loggedIn;
    }

    void Player::setLoggedIn(bool loggedIn)
    {
        _loggedIn = loggedIn;
    }

    bool Player::ready(void) const
    {
        return _ready;
    }

    void Player::setReady(bool ready)
    {
        _ready = ready;
    }

    uint32_t Player::roomId(void) const
    {
        return _roomId;
    }

    void Player::setRoomId(uint32_t roomId)
    {
        _roomId = roomId;
    }

    uint32_t Player::entityId(void) const
    {
        return _entityId;
    }

    void Player::setEntityId(uint32_t entityId)
    {
        _entityId = entityId;
    }

    aer::math::Vec2f Player::position(void) const
    {
        return _position;
    }

    void Player::setPosition(const aer::math::Vec2f &position)
    {
        _position = position;
    }

    aer::math::Vec2f Player::velocity(void) const
    {
        return _velocity;
    }

    void Player::setVelocity(const aer::math::Vec2f &velocity)
    {
        _velocity = velocity;
    }

    uint8_t Player::inputMask(void) const
    {
        return _inputMask;
    }

    void Player::setInputMask(uint8_t mask)
    {
        _inputMask = mask;
    }
}
