/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** SettingsMenuSystem.cpp
*/

#include "Systems/SettingsMenuSystem.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/Logger.hpp"

namespace Client::Systems {

void SettingsMenuSystem::update(float dt) {
    (void)dt;
    
    // Handle mouse input for buttons/sliders/dropdowns
    sf::Vector2i mousePos = sf::Mouse::getPosition(_window);
    (void)mousePos;
    
    handleSliderInteraction();
    handleDropdownInteraction();
    
    // Handle key rebinding if waiting for input
    if (_waitingForKeyInput) {
        // TODO: Implement key rebinding with event system
        // handleKeyBindingInput();
    }
}

void SettingsMenuSystem::handleKeyBindingInput() {
    // TODO: Implement proper key rebinding
    // This requires event handling system to work properly
    _waitingForKeyInput = false;
}

void SettingsMenuSystem::handleSliderInteraction() {
    // TODO: Implement slider dragging for volume controls
    // This will be handled by checking mouse button state and position
}

void SettingsMenuSystem::handleDropdownInteraction() {
    // TODO: Implement dropdown menu interaction for language and accessibility
}

}  // namespace Client::Systems
