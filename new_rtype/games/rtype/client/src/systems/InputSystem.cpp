/**
 * File   : InputSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/InputSystem.hpp"

namespace rtp::client::systems
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    InputSystem::InputSystem(engine::ecs::Registry& worldRegistry,
                             engine::ecs::Registry& uiRegistry,
                             Settings& settings,
                             engine::net::INetwork& network,
                             uint32_t sessionId)
        : _worldRegistry(worldRegistry),
          _uiRegistry(uiRegistry),
          _settings(settings),
          _network(network),
          _sessionId(sessionId)
    {
    }

    void InputSystem::update(float)
    {
        if (_sessionId == 0) {
            return;
        }

        if (auto inputsOpt = _uiRegistry.getComponents<engine::ecs::components::TextInput>()) {
            auto &inputs = inputsOpt.value().get();
            for (const auto &e : inputs.entities()) {
                if (inputs[e].isFocused) {
                    if (_lastMask != 0) {
                        rtp::net::Packet packet(rtp::net::OpCode::InputTick);
                        rtp::net::InputPayload payload{0};
                        packet << payload;
                        const auto data = packet.serialize();
                        _network.sendPacket(_sessionId, data, engine::net::NetChannel::UDP);
                        _lastMask = 0;
                    }
                    return;
                }
            }
        }

        const auto &input = engine::input::Input::instance();
        uint8_t mask = 0;

        if (input.isKeyDown(engine::input::KeyCode::W) ||
            input.isKeyDown(engine::input::KeyCode::Up)) {
            mask |= InputBits::MoveUp;
        }
        if (input.isKeyDown(engine::input::KeyCode::S) ||
            input.isKeyDown(engine::input::KeyCode::Down)) {
            mask |= InputBits::MoveDown;
        }
        if (input.isKeyDown(engine::input::KeyCode::A) ||
            input.isKeyDown(engine::input::KeyCode::Left)) {
            mask |= InputBits::MoveLeft;
        }
        if (input.isKeyDown(engine::input::KeyCode::D) ||
            input.isKeyDown(engine::input::KeyCode::Right)) {
            mask |= InputBits::MoveRight;
        }
        if (input.isKeyDown(engine::input::KeyCode::Space)) {
            mask |= InputBits::Shoot;
        }
        if (input.isKeyDown(engine::input::KeyCode::R)) {
            mask |= InputBits::Reload;
        }

        if (mask == _lastMask) {
            return;
        }

        _lastMask = mask;

        rtp::net::Packet packet(rtp::net::OpCode::InputTick);
        rtp::net::InputPayload payload{mask};
        packet << payload;
        const auto data = packet.serialize();
        _network.sendPacket(_sessionId, data, engine::net::NetChannel::UDP);
    }

    void InputSystem::setSessionId(uint32_t sessionId)
    {
        _sessionId = sessionId;
    }
} // namespace rtp::client::systems
