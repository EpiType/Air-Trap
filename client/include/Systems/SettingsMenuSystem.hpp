/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** SettingsMenuSystem - Handles settings menu interactions
*/

#pragma once

#include <SFML/Graphics.hpp>
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/ISystem.hpp"
#include "Core/Settings.hpp"

namespace Client::Systems {

enum class SettingsTab {
    KeyBindings,
    Audio,
    Language,
    Accessibility
};

/**
 * @class SettingsMenuSystem
 * @brief System for handling settings menu interactions
 */
class SettingsMenuSystem : public rtp::ecs::ISystem {
public:
    SettingsMenuSystem(rtp::ecs::Registry& registry, sf::RenderWindow& window, Core::Settings& settings)
        : _registry(registry), _window(window), _settings(settings) {}

    void update(float dt) override;
    
    void setActiveTab(SettingsTab tab) { _activeTab = tab; }
    SettingsTab getActiveTab() const { return _activeTab; }

private:
    void handleKeyBindingInput();
    void handleSliderInteraction();
    void handleDropdownInteraction();
    
    rtp::ecs::Registry& _registry;
    sf::RenderWindow& _window;
    Core::Settings& _settings;
    
    SettingsTab _activeTab{SettingsTab::Audio};
    bool _waitingForKeyInput{false};
    Core::KeyAction _keyToRebind;
};

}  // namespace Client::Systems
