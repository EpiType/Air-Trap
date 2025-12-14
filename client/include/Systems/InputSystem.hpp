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

#include <SFML/Window/Event.hpp>

namespace rtp::client {
    class InputSystem : public rtp::ecs::ISystem {
    public:
        explicit InputSystem(rtp::ecs::Registry& r) : _r(r) {}

        void update(float dt) override {
            auto view = _r.zipView<rtp::ecs::components::Velocity, rtp::ecs::components::Controllable>();

            for (auto&& [vel, _] : view) {
                vel.direction = {0.0f, 0.0f};

                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up)) {
                    vel.direction.y -= 1.0f;
                    rtp::log::info("Up key pressed");
                }
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down))
                    vel.direction.y += 1.0f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left))
                    vel.direction.x -= 1.0f;
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right))
                    vel.direction.x += 1.0f;
            }
        }
    private:
        rtp::ecs::Registry& _r;
    };
}
#endif // INPUT_SYSTEM_HPP