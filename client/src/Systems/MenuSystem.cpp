/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** MenuSystem implementation
*/

#include "Systems/MenuSystem.hpp"
#include "RType/Logger.hpp"

namespace Client::Systems {

void MenuSystem::update(float dt) {
    (void)dt;
    
    auto buttonsCheck = _registry.getComponents<rtp::ecs::components::ui::Button>();
    if (!buttonsCheck) {
        // Les entités ont été détruites, ne rien faire
        return;
    }
    
    sf::Vector2i mousePos = sf::Mouse::getPosition(_window);
    
    handleMouseMove(mousePos);
    
    buttonsCheck = _registry.getComponents<rtp::ecs::components::ui::Button>();
    if (!buttonsCheck) {
        return;
    }
    
    // Gérer les clics sur tout (boutons, sliders, dropdowns)
    static bool wasPressed = false;
    bool isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);
    
    if (isPressed && !wasPressed) {
        handleMouseClick(mousePos);
    }
    
    wasPressed = isPressed;
}

void MenuSystem::handleMouseMove(const sf::Vector2i& mousePos) {
    auto buttonsResult = _registry.getComponents<rtp::ecs::components::ui::Button>();
    if (!buttonsResult)
        return;
    
    auto& buttons = buttonsResult.value().get();
    for (const auto& entity : buttons.getEntities()) {
        auto& button = buttons[entity];
        
        if (isMouseOverButton(button, mousePos)) {
            button.state = rtp::ecs::components::ui::ButtonState::Hovered;
        } else {
            button.state = rtp::ecs::components::ui::ButtonState::Idle;
        }
    }
}

void MenuSystem::handleMouseClick(const sf::Vector2i& mousePos) {
    auto buttonsResult = _registry.getComponents<rtp::ecs::components::ui::Button>();
    if (buttonsResult) {
        auto& buttons = buttonsResult.value().get();
        
        std::vector<std::pair<rtp::ecs::Entity, std::function<void()>>> buttonCallbacks;
        for (const auto& entity : buttons.getEntities()) {
            auto& button = buttons[entity];
            if (isMouseOverButton(button, mousePos)) {
                button.state = rtp::ecs::components::ui::ButtonState::Pressed;
                
                if (button.onClick) {
                    buttonCallbacks.emplace_back(entity, button.onClick);
                }
                
                rtp::log::info("Button '{}' clicked", button.text);
                break;  // On ne peut cliquer qu'un seul bouton
            }
        }
        
        // Exécuter les callbacks APRÈS avoir fini d'accéder aux composants
        for (auto& [entity, callback] : buttonCallbacks) {
            callback();
            return;  // Sortir après le premier callback
        }
    }
    
    auto slidersResult = _registry.getComponents<rtp::ecs::components::ui::Slider>();
    if (slidersResult) {
        auto& sliders = slidersResult.value().get();
        for (const auto& entity : sliders.getEntities()) {
            auto& slider = sliders[entity];
            
            if (isMouseOverSlider(slider, mousePos)) {
                slider.isDragging = true;
                updateSliderValue(slider, mousePos);
                rtp::log::info("Slider clicked at value: {}", slider.currentValue);
                return;
            }
        }
    }
    
    auto dropdownsResult = _registry.getComponents<rtp::ecs::components::ui::Dropdown>();
    if (dropdownsResult) {
        auto& dropdowns = dropdownsResult.value().get();
        for (const auto& entity : dropdowns.getEntities()) {
            auto& dropdown = dropdowns[entity];
            
            if (isMouseOverDropdown(dropdown, mousePos)) {
                dropdown.isOpen = !dropdown.isOpen;
                rtp::log::info("Dropdown toggled: {}", dropdown.isOpen ? "open" : "closed");
                return;
            }
            
            // Si ouvert, vérifier les options
            if (dropdown.isOpen) {
                int optionIndex = getDropdownOptionAtMouse(dropdown, mousePos);
                if (optionIndex >= 0) {
                    dropdown.selectedIndex = optionIndex;
                    dropdown.isOpen = false;
                    
                    if (dropdown.onSelect) {
                        dropdown.onSelect(optionIndex);
                    }
                    
                    rtp::log::info("Dropdown option {} selected", optionIndex);
                    return;
                }
            }
        }
    }
}

bool MenuSystem::isMouseOverButton(
    const rtp::ecs::components::ui::Button& button,
    const sf::Vector2i& mousePos) 
{
    return mousePos.x >= button.position.x &&
           mousePos.x <= button.position.x + button.size.x &&
           mousePos.y >= button.position.y &&
           mousePos.y <= button.position.y + button.size.y;
}

bool MenuSystem::isMouseOverSlider(
    const rtp::ecs::components::ui::Slider& slider,
    const sf::Vector2i& mousePos)
{
    return mousePos.x >= slider.position.x &&
           mousePos.x <= slider.position.x + slider.size.x &&
           mousePos.y >= slider.position.y &&
           mousePos.y <= slider.position.y + slider.size.y;
}

bool MenuSystem::isMouseOverDropdown(
    const rtp::ecs::components::ui::Dropdown& dropdown,
    const sf::Vector2i& mousePos)
{
    return mousePos.x >= dropdown.position.x &&
           mousePos.x <= dropdown.position.x + dropdown.size.x &&
           mousePos.y >= dropdown.position.y &&
           mousePos.y <= dropdown.position.y + dropdown.size.y;
}

void MenuSystem::updateSliderValue(
    rtp::ecs::components::ui::Slider& slider,
    const sf::Vector2i& mousePos)
{
    float relativeX = mousePos.x - slider.position.x;
    float percentage = std::clamp(relativeX / slider.size.x, 0.0f, 1.0f);
    
    slider.currentValue = slider.minValue + 
                         percentage * (slider.maxValue - slider.minValue);
    
    if (slider.onChange) {
        slider.onChange(slider.currentValue);
    }
}

int MenuSystem::getDropdownOptionAtMouse(
    const rtp::ecs::components::ui::Dropdown& dropdown,
    const sf::Vector2i& mousePos)
{
    float optionHeight = dropdown.size.y;
    float startY = dropdown.position.y + dropdown.size.y;
    
    for (size_t i = 0; i < dropdown.options.size(); ++i) {
        float optionY = startY + i * optionHeight;
        
        if (mousePos.x >= dropdown.position.x &&
            mousePos.x <= dropdown.position.x + dropdown.size.x &&
            mousePos.y >= optionY &&
            mousePos.y <= optionY + optionHeight) {
            return static_cast<int>(i);
        }
    }
    
    return -1;
}

}