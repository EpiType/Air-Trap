/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** MenuSystem
*/

#ifndef MENUSYSTEM_HPP_
#define MENUSYSTEM_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/Logger.hpp"

namespace Client::Systems {

/**
 * @class MenuSystem
 * @brief System responsible for handling menu interactions
 */
class MenuSystem : public rtp::ecs::ISystem {
public:
    MenuSystem(rtp::ecs::Registry& registry, sf::RenderWindow& window)
        : _registry(registry), _window(window) {}

    void update(float dt) override;

private:
    void handleMouseMove(const sf::Vector2i& mousePos);
    void handleMouseClick(const sf::Vector2i& mousePos);
    bool isMouseOverButton(const rtp::ecs::components::ui::Button& button, 
                          const sf::Vector2i& mousePos);

    rtp::ecs::Registry& _registry;
    sf::RenderWindow& _window;
};

}  // namespace Client::Systems

#endif /* !MENUSYSTEM_HPP_ */
