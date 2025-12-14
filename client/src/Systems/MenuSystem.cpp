/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** MenuSystem.cpp
*/

#include "Systems/MenuSystem.hpp"
#include "RType/ECS/ZipView.hpp"

namespace Client::Systems {

void MenuSystem::update(float dt) {
    (void)dt;
    // Get mouse position
    sf::Vector2i mousePos = sf::Mouse::getPosition(_window);
    
    // Handle mouse movement (hover detection)
    handleMouseMove(mousePos);
    
    // Handle mouse clicks
    if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        handleMouseClick(mousePos);
    }
}

void MenuSystem::handleMouseMove(const sf::Vector2i& mousePos) {
    auto buttonsResult = _registry.getComponents<rtp::ecs::components::ui::Button>();
    if (!buttonsResult) return;
    
    auto& buttons = buttonsResult.value().get();
    for (const auto& entity : buttons.getEntities()) {
        auto& button = buttons[entity];
        
        if (isMouseOverButton(button, mousePos)) {
            if (button.state != rtp::ecs::components::ui::ButtonState::Pressed) {
                button.state = rtp::ecs::components::ui::ButtonState::Hovered;
            }
        } else {
            button.state = rtp::ecs::components::ui::ButtonState::Idle;
        }
    }
}

void MenuSystem::handleMouseClick(const sf::Vector2i& mousePos) {
    auto buttonsResult = _registry.getComponents<rtp::ecs::components::ui::Button>();
    if (!buttonsResult) return;
    
    auto& buttons = buttonsResult.value().get();
    for (const auto& entity : buttons.getEntities()) {
        auto& button = buttons[entity];
        
        if (isMouseOverButton(button, mousePos)) {
            button.state = rtp::ecs::components::ui::ButtonState::Pressed;
            
            // Execute callback
            if (button.onClick) {
                button.onClick();
            }
            
            rtp::log::info("Button '{}' clicked", button.text);
        }
    }
}

bool MenuSystem::isMouseOverButton(
    const rtp::ecs::components::ui::Button& button,
    const sf::Vector2i& mousePos) 
{
    float mouseX = static_cast<float>(mousePos.x);
    float mouseY = static_cast<float>(mousePos.y);
    
    return mouseX >= button.position.x &&
           mouseX <= button.position.x + button.size.x &&
           mouseY >= button.position.y &&
           mouseY <= button.position.y + button.size.y;
}

}  // namespace Client::Systems