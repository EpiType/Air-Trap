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
#include "RType/ECS/Components/UI/Slider.hpp"
#include "RType/ECS/Components/UI/Dropdown.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/UI/TextInput.hpp"
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

    void handleEvent(const sf::Event& event);

private:
    void handleMouseMove(const sf::Vector2i& mousePos);
    void handleMouseClick(const sf::Vector2i& mousePos);
    
    bool isMouseOverButton(const rtp::ecs::components::ui::Button& button, 
                          const sf::Vector2i& mousePos);
    
    bool isMouseOverSlider(const rtp::ecs::components::ui::Slider& slider,
                          const sf::Vector2i& mousePos);
    
    bool isMouseOverDropdown(const rtp::ecs::components::ui::Dropdown& dropdown,
                            const sf::Vector2i& mousePos);
    
    void updateSliderValue(rtp::ecs::components::ui::Slider& slider,
                          const sf::Vector2i& mousePos);
    
    int getDropdownOptionAtMouse(const rtp::ecs::components::ui::Dropdown& dropdown,
                                const sf::Vector2i& mousePos);

    bool isMouseOverTextInput(const rtp::ecs::components::ui::TextInput& input, const sf::Vector2i& mousePos) const;
    void focusTextInputAt(const sf::Vector2i& mousePos);
    void clearAllTextInputFocus();

    void handleTextEntered(std::uint32_t unicode);
    void handleKeyPressed(sf::Keyboard::Key key);

    rtp::ecs::Registry& _registry;
    sf::RenderWindow& _window;
};

}  // namespace Client::Systems

#endif /* !MENUSYSTEM_HPP_ */
