/**
 * File   : InputSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "Systems/InputSystem.hpp"
#include <SFML/Graphics.hpp>

namespace rtp::client {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    InputSystem::InputSystem(rtp::ecs::Registry& r,
                         Client::Core::Settings& settings,
                         ClientNetwork& net,
                         sf::RenderWindow& window)
        : _r(r), _settings(settings), _net(net), _window(window) {}

    void InputSystem::update(float)
    {
        if (!_window.hasFocus())
            return;
        if (!_net.isUdpReady())
            return;

        uint8_t mask = 0;

        if (sf::Keyboard::isKeyPressed(_settings.getKey(Client::Core::KeyAction::MoveUp)))
            mask |= InputBits::MoveUp;
        if (sf::Keyboard::isKeyPressed(_settings.getKey(Client::Core::KeyAction::MoveDown)))
            mask |= InputBits::MoveDown;
        if (sf::Keyboard::isKeyPressed(_settings.getKey(Client::Core::KeyAction::MoveLeft)))
            mask |= InputBits::MoveLeft;
        if (sf::Keyboard::isKeyPressed(_settings.getKey(Client::Core::KeyAction::MoveRight)))
            mask |= InputBits::MoveRight;

        if (mask == _lastMask)
            return;

        _lastMask = mask;

        log::info("Sending InputTick with mask: {}", (int)mask);

        rtp::net::Packet p(rtp::net::OpCode::InputTick);
        rtp::net::InputPayload payload{ mask };
        p << payload;

        _net.sendPacket(p, rtp::net::NetworkMode::UDP);
    }

} // namespace rtp::client