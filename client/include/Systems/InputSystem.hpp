/**
 * File   : InputSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef INPUT_SYSTEM_HPP
#define INPUT_SYSTEM_HPP

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"

#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/Controllable.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "Core/Settings.hpp"

#include <SFML/Window/Event.hpp>

namespace rtp::client {
    class InputSystem : public rtp::ecs::ISystem {
    public:
        explicit InputSystem(rtp::ecs::Registry& r, Client::Core::Settings& settings) 
            : _r(r), _settings(settings) {}

        void update(float dt) override {
            (void)dt;
            
            auto view = _r.zipView<rtp::ecs::components::Transform, rtp::ecs::components::Controllable>();

            for (auto&& [transform, _] : view) {
                if (sf::Keyboard::isKeyPressed(_settings.getKey(Client::Core::KeyAction::MoveUp))) {
                    transform.position.y -= 5.0f;
                }
                if (sf::Keyboard::isKeyPressed(_settings.getKey(Client::Core::KeyAction::MoveDown))) {
                    transform.position.y += 5.0f;
                }
                if (sf::Keyboard::isKeyPressed(_settings.getKey(Client::Core::KeyAction::MoveLeft))) {
                    transform.position.x -= 5.0f;
                }
                if (sf::Keyboard::isKeyPressed(_settings.getKey(Client::Core::KeyAction::MoveRight))) {
                    transform.position.x += 5.0f;
                }
            }
        }

    private:
        rtp::ecs::Registry& _r;
        Client::Core::Settings& _settings;
    };
}

#endif // INPUT_SYSTEM_HPP