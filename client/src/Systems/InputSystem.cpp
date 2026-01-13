/**
 * File   : InputSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "Systems/InputSystem.hpp"
#include "RType/ECS/Components/UI/TextInput.hpp"
#include <SFML/Graphics.hpp>

namespace rtp::client {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    InputSystem::InputSystem(rtp::ecs::Registry& r,
                         rtp::ecs::Registry& uiRegistry,
                         Settings& settings,
                         ClientNetwork& net,
                         sf::RenderWindow& window)
        : _r(r), _uiRegistry(uiRegistry), _settings(settings), _net(net), _window(window) {}

    void InputSystem::update(float)
    {
        if (!_window.hasFocus())
            return;
        if (!_net.isUdpReady())
            return;

        if (auto inputsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::TextInput>()) {
            auto &inputs = inputsOpt.value().get();
            for (const auto &e : inputs.entities()) {
                if (inputs[e].isFocused) {
                    if (_lastMask != 0) {
                        rtp::net::Packet p(rtp::net::OpCode::InputTick);
                        rtp::net::InputPayload payload{0};
                        p << payload;
                        _net.sendPacket(p, rtp::net::NetworkMode::UDP);
                        _lastMask = 0;
                    }
                    return;
                }
            }
        }

        uint8_t mask = 0;

        if (sf::Keyboard::isKeyPressed(_settings.getKey(KeyAction::MoveUp)))
            mask |= InputBits::MoveUp;
        if (sf::Keyboard::isKeyPressed(_settings.getKey(KeyAction::MoveDown)))
            mask |= InputBits::MoveDown;
        if (sf::Keyboard::isKeyPressed(_settings.getKey(KeyAction::MoveLeft)))
            mask |= InputBits::MoveLeft;
        if (sf::Keyboard::isKeyPressed(_settings.getKey(KeyAction::MoveRight)))
            mask |= InputBits::MoveRight;
        if (sf::Keyboard::isKeyPressed(_settings.getKey(KeyAction::Shoot)))
            mask |= InputBits::Shoot;
        if (sf::Keyboard::isKeyPressed(_settings.getKey(KeyAction::Reload)))
            mask |= InputBits::Reload;

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
