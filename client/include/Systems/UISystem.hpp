/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** UISystem
*/

#ifndef UISystem_HPP_
#define UISystem_HPP_

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Components/UI/Slider.hpp"
#include "RType/ECS/Components/UI/Dropdown.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/UI/TextInput.hpp"
#include "Core/Settings.hpp"
#include "RType/Logger.hpp"

namespace rtp::client 
{
    /**
     * @class UISystem
     * @brief System responsible for handling menu interactions
     */
    class UISystem : public rtp::ecs::ISystem {
        public:
            UISystem(rtp::ecs::Registry& registry, sf::RenderWindow& window, Settings& settings)
                : _registry(registry), _window(window), _settings(settings), 
                  _gamepadCursorPos(640.0f, 360.0f) {}

            void update(float dt) override;

            void handleEvent(const sf::Event& event);
            void renderGamepadCursor(sf::RenderWindow& window);

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

            void updateGamepadCursor(float dt);
            void handleGamepadInput();

            rtp::ecs::Registry& _registry;
            sf::RenderWindow& _window;
            Settings& _settings;
            
            sf::Vector2f _gamepadCursorPos;
            bool _gamepadMode{false};
            sf::Clock _gamepadButtonClock;
    };
}  // namespace Client::Systems

#endif /* !UISystem_HPP_ */
