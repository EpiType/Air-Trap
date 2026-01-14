/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** UISystem
*/

#include "Systems/UISystem.hpp"

#include "RType/Logger.hpp"
#include <SFML/Window/Joystick.hpp>
#include <cmath>

namespace rtp::client
{
    void UISystem::update(float dt)
    {
        // Handle gamepad cursor movement
        updateGamepadCursor(dt);
        handleGamepadInput();

        auto buttonsCheck =
            _registry.getComponents<rtp::ecs::components::ui::Button>();
        if (!buttonsCheck) {
            return;
        }

        // Use gamepad cursor if in gamepad mode, otherwise mouse
        sf::Vector2i cursorPos = _gamepadMode 
            ? sf::Vector2i(static_cast<int>(_gamepadCursorPos.x), static_cast<int>(_gamepadCursorPos.y))
            : sf::Mouse::getPosition(_window);

        handleMouseMove(cursorPos);

        buttonsCheck =
            _registry.getComponents<rtp::ecs::components::ui::Button>();
        if (!buttonsCheck) {
            return;
        }

        static bool wasPressed = false;
        bool isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if (isPressed && !wasPressed) {
            handleMouseClick(cursorPos);
        }

        wasPressed = isPressed;
    }

    void UISystem::handleEvent(const sf::Event &event)
    {
        if (const auto *te = event.getIf<sf::Event::TextEntered>()) {
            handleTextEntered(te->unicode);
        }
        if (const auto *kp = event.getIf<sf::Event::KeyPressed>()) {
            handleKeyPressed(kp->code);
        }
    }

    void UISystem::handleMouseMove(const sf::Vector2i &mousePos)
    {
        auto buttonsResult =
            _registry.getComponents<rtp::ecs::components::ui::Button>();
        if (!buttonsResult)
            return;

        auto &buttons = buttonsResult.value().get();
        for (const auto &entity : buttons.entities()) {
            auto &button = buttons[entity];

            if (isMouseOverButton(button, mousePos)) {
                button.state = rtp::ecs::components::ui::ButtonState::Hovered;
            } else {
                button.state = rtp::ecs::components::ui::ButtonState::Idle;
            }
        }
    }

    void UISystem::handleMouseClick(const sf::Vector2i &mousePos)
    {
        auto dropdownsResult =
            _registry.getComponents<rtp::ecs::components::ui::Dropdown>();
        if (dropdownsResult) {
            auto &dropdowns = dropdownsResult.value().get();

            for (const auto &entity : dropdowns.entities()) {
                auto &dropdown = dropdowns[entity];

                if (dropdown.isOpen) {
                    int optionIndex =
                        getDropdownOptionAtMouse(dropdown, mousePos);
                    if (optionIndex >= 0) {
                        dropdown.selectedIndex = optionIndex;
                        dropdown.isOpen = false;

                        if (dropdown.onSelect) {
                            dropdown.onSelect(optionIndex);
                        }

                        rtp::log::info("Dropdown option {} selected",
                                       optionIndex);
                        return;
                    }
                }
            }

            for (const auto &entity : dropdowns.entities()) {
                auto &dropdown = dropdowns[entity];

                if (isMouseOverDropdown(dropdown, mousePos)) {
                    dropdown.isOpen = !dropdown.isOpen;
                    rtp::log::info("Dropdown toggled: {}",
                                   dropdown.isOpen ? "open" : "closed");

                    for (const auto &otherEntity : dropdowns.entities()) {
                        if (otherEntity != entity) {
                            dropdowns[otherEntity].isOpen = false;
                        }
                    }
                    return;
                }
            }

            bool hadOpenDropdown = false;
            for (const auto &entity : dropdowns.entities()) {
                if (dropdowns[entity].isOpen) {
                    dropdowns[entity].isOpen = false;
                    hadOpenDropdown = true;
                }
            }
            if (hadOpenDropdown) {
                return;
            }
        }

        {
            auto inputsResult =
                _registry.getComponents<rtp::ecs::components::ui::TextInput>();
            if (inputsResult) {
                auto &inputs = inputsResult.value().get();

                auto isMouseOverTextInput =
                    [&](const rtp::ecs::components::ui::TextInput &in) -> bool {
                    return mousePos.x >=
                           in.position.x &&
                           mousePos.x <=
                           in.position.x +
                           in.size.x &&
                           mousePos.y >=
                           in.position.y &&
                           mousePos.y <=
                           in.position.y +
                           in.size.y;
                };

                bool clickedOnAnyInput = false;

                for (const auto &entity : inputs.entities()) {
                    if (isMouseOverTextInput(inputs[entity])) {
                        clickedOnAnyInput = true;
                        break;
                    }
                }

                if (clickedOnAnyInput) {
                    for (const auto &entity : inputs.entities()) {
                        auto &in = inputs[entity];
                        const bool over = isMouseOverTextInput(in);
                        in.isFocused = over;

                        if (over) {
                            in.showCursor = true;
                            in.blinkTimer = 0.0f;
                        } else {
                            in.showCursor = false;
                            in.blinkTimer = 0.0f;
                        }
                    }

                    rtp::log::info("TextInput focused");
                    return;
                } else {
                    bool hadFocus = false;
                    for (const auto &entity : inputs.entities()) {
                        auto &in = inputs[entity];
                        if (in.isFocused)
                            hadFocus = true;
                        in.isFocused = false;
                        in.showCursor = false;
                        in.blinkTimer = 0.0f;
                    }
                    if (hadFocus) {
                        rtp::log::info("TextInput unfocused");
                    }
                }
            }
        }

        auto buttonsResult =
            _registry.getComponents<rtp::ecs::components::ui::Button>();
        if (buttonsResult) {
            auto &buttons = buttonsResult.value().get();

            std::vector<std::pair<rtp::ecs::Entity, std::function<void()>>>
                buttonCallbacks;
            for (const auto &entity : buttons.entities()) {
                auto &button = buttons[entity];
                if (!button.onClick && button.text.empty()) {
                    continue;
                }
                if (isMouseOverButton(button, mousePos)) {
                    button.state =
                        rtp::ecs::components::ui::ButtonState::Pressed;

                    if (button.onClick) {
                        buttonCallbacks.emplace_back(entity, button.onClick);
                    }

                    rtp::log::info("Button '{}' clicked", button.text);
                    break;
                }
            }

            for (auto &[entity, callback] : buttonCallbacks) {
                (void)entity;
                callback();
                return;
            }
        }

        auto slidersResult =
            _registry.getComponents<rtp::ecs::components::ui::Slider>();
        if (slidersResult) {
            auto &sliders = slidersResult.value().get();
            for (const auto &entity : sliders.entities()) {
                auto &slider = sliders[entity];

                if (isMouseOverSlider(slider, mousePos)) {
                    slider.isDragging = true;
                    updateSliderValue(slider, mousePos);
                    rtp::log::info("Slider clicked at value: {}",
                                   slider.currentValue);
                    return;
                }
            }
        }
    }

    bool UISystem::isMouseOverButton(
        const rtp::ecs::components::ui::Button &button,
        const sf::Vector2i &mousePos)
    {
        return mousePos.x >=
               button.position.x &&
               mousePos.x <=
               button.position.x +
               button.size.x &&
               mousePos.y >=
               button.position.y &&
               mousePos.y <=
               button.position.y +
               button.size.y;
    }

    bool UISystem::isMouseOverSlider(
        const rtp::ecs::components::ui::Slider &slider,
        const sf::Vector2i &mousePos)
    {
        return mousePos.x >=
               slider.position.x &&
               mousePos.x <=
               slider.position.x +
               slider.size.x &&
               mousePos.y >=
               slider.position.y &&
               mousePos.y <=
               slider.position.y +
               slider.size.y;
    }

    bool UISystem::isMouseOverDropdown(
        const rtp::ecs::components::ui::Dropdown &dropdown,
        const sf::Vector2i &mousePos)
    {
        return mousePos.x >=
               dropdown.position.x &&
               mousePos.x <=
               dropdown.position.x +
               dropdown.size.x &&
               mousePos.y >=
               dropdown.position.y &&
               mousePos.y <=
               dropdown.position.y +
               dropdown.size.y;
    }

    void UISystem::updateSliderValue(rtp::ecs::components::ui::Slider &slider,
                                       const sf::Vector2i &mousePos)
    {
        float relativeX = mousePos.x - slider.position.x;
        float percentage = std::clamp(relativeX / slider.size.x, 0.0f, 1.0f);

        slider.currentValue =
            slider.minValue + percentage * (slider.maxValue - slider.minValue);

        if (slider.onChange) {
            slider.onChange(slider.currentValue);
        }
    }

    int UISystem::getDropdownOptionAtMouse(
        const rtp::ecs::components::ui::Dropdown &dropdown,
        const sf::Vector2i &mousePos)
    {
        float optionHeight = dropdown.size.y;
        float startY = dropdown.position.y + dropdown.size.y;

        for (size_t i = 0; i < dropdown.options.size(); ++i) {
            float optionY = startY + i * optionHeight;

            if (mousePos.x >=
                dropdown.position.x &&
                mousePos.x <=
                dropdown.position.x +
                dropdown.size.x &&
                mousePos.y >=
                optionY &&
                mousePos.y <=
                optionY +
                optionHeight) {
                return static_cast<int>(i);
            }
        }

        return -1;
    }

    bool UISystem::isMouseOverTextInput(
        const rtp::ecs::components::ui::TextInput &input,
        const sf::Vector2i &mousePos) const
    {
        return mousePos.x >=
               input.position.x &&
               mousePos.x <=
               input.position.x +
               input.size.x &&
               mousePos.y >=
               input.position.y &&
               mousePos.y <=
               input.position.y +
               input.size.y;
    }

    void UISystem::clearAllTextInputFocus()
    {
        auto inputsResult =
            _registry.getComponents<rtp::ecs::components::ui::TextInput>();
        if (!inputsResult)
            return;

        auto &inputs = inputsResult.value().get();
        for (const auto &e : inputs.entities()) {
            inputs[e].isFocused = false;
            inputs[e].showCursor = false;
            inputs[e].blinkTimer = 0.0f;
        }
    }

    void UISystem::focusTextInputAt(const sf::Vector2i &mousePos)
    {
        auto inputsResult =
            _registry.getComponents<rtp::ecs::components::ui::TextInput>();
        if (!inputsResult)
            return;

        auto &inputs = inputsResult.value().get();

        bool focusedOne = false;
        for (const auto &e : inputs.entities()) {
            auto &in = inputs[e];
            if (!focusedOne && isMouseOverTextInput(in, mousePos)) {
                in.isFocused = true;
                in.showCursor = true;
                in.blinkTimer = 0.0f;
                focusedOne = true;
            } else {
                in.isFocused = false;
                in.showCursor = false;
                in.blinkTimer = 0.0f;
            }
        }
    }

    void UISystem::handleTextEntered(std::uint32_t unicode)
    {
        auto inputsResult =
            _registry.getComponents<rtp::ecs::components::ui::TextInput>();
        if (!inputsResult)
            return;

        auto &inputs = inputsResult.value().get();

        if (unicode == 8) {
            for (const auto &e : inputs.entities()) {
                auto &in = inputs[e];
                if (!in.isFocused)
                    continue;
                if (!in.value.empty()) {
                    in.value.pop_back();
                    if (in.onChange)
                        in.onChange(in.value);
                }
            }
            return;
        }
        if (unicode == 13) {
            for (const auto &e : inputs.entities()) {
                auto &in = inputs[e];
                if (!in.isFocused)
                    continue;
                if (in.onSubmit)
                    in.onSubmit(in.value);
            }
            return;
        }
        if (unicode < 32) {
            return;
        }

        if (unicode > 127) {
            return;
        }

        const char c = static_cast<char>(unicode);

        for (const auto &e : inputs.entities()) {
            auto &in = inputs[e];
            if (!in.isFocused)
                continue;

            if (in.value.size() >= in.maxLength)
                return;

            in.value.push_back(c);
            if (in.onChange)
                in.onChange(in.value);
            return;
        }
    }

    void UISystem::handleKeyPressed(sf::Keyboard::Key key)
    {
        auto inputsResult =
            _registry.getComponents<rtp::ecs::components::ui::TextInput>();
        if (!inputsResult)
            return;

        auto &inputs = inputsResult.value().get();

        if (key == sf::Keyboard::Key::Escape) {
            clearAllTextInputFocus();
            return;
        }

        if (key == sf::Keyboard::Key::Backspace) {
            for (const auto &e : inputs.entities()) {
                auto &in = inputs[e];
                if (!in.isFocused)
                    continue;

                if (!in.value.empty()) {
                    in.value.pop_back();
                    if (in.onChange)
                        in.onChange(in.value);
                }
                return;
            }
        }

        if (key == sf::Keyboard::Key::Enter) {
            for (const auto &e : inputs.entities()) {
                auto &in = inputs[e];
                if (!in.isFocused)
                    continue;

                if (in.onSubmit)
                    in.onSubmit(in.value);
                return;
            }
        }
    }

    void UISystem::updateGamepadCursor(float dt)
    {
        if (!_settings.getGamepadEnabled() || !sf::Joystick::isConnected(0))
            return;

        float deadzone = _settings.getGamepadDeadzone();
        float speed = _settings.getGamepadCursorSpeed() * 200.0f * dt;

        // Read left stick
        float axisX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::X);
        float axisY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::Y);

        // If stick is being used, enable gamepad mode
        if (std::abs(axisX) > deadzone || std::abs(axisY) > deadzone) {
            _gamepadMode = true;
            
            if (std::abs(axisX) > deadzone)
                _gamepadCursorPos.x += (axisX / 100.0f) * speed;
            if (std::abs(axisY) > deadzone)
                _gamepadCursorPos.y += (axisY / 100.0f) * speed;
        }

        // D-pad alternative
        float povX = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovX);
        float povY = sf::Joystick::getAxisPosition(0, sf::Joystick::Axis::PovY);
        
        if (std::abs(povX) > 50 || std::abs(povY) > 50) {
            _gamepadMode = true;
            
            if (povX < -50)
                _gamepadCursorPos.x -= speed * 2.0f;
            if (povX > 50)
                _gamepadCursorPos.x += speed * 2.0f;
            if (povY > 50)
                _gamepadCursorPos.y -= speed * 2.0f;
            if (povY < -50)
                _gamepadCursorPos.y += speed * 2.0f;
        }

        // Clamp to window bounds
        _gamepadCursorPos.x = std::clamp(_gamepadCursorPos.x, 0.0f, static_cast<float>(_window.getSize().x));
        _gamepadCursorPos.y = std::clamp(_gamepadCursorPos.y, 0.0f, static_cast<float>(_window.getSize().y));

        // If mouse moves, disable gamepad mode
        static sf::Vector2i lastMousePos = sf::Mouse::getPosition(_window);
        sf::Vector2i currentMousePos = sf::Mouse::getPosition(_window);
        if (currentMousePos != lastMousePos) {
            _gamepadMode = false;
        }
        lastMousePos = currentMousePos;
    }

    void UISystem::handleGamepadInput()
    {
        if (!_settings.getGamepadEnabled() || !sf::Joystick::isConnected(0) || !_gamepadMode)
            return;

        // Check validate button (A button by default)
        static bool wasValidatePressed = false;
        bool isValidatePressed = sf::Joystick::isButtonPressed(0, _settings.getGamepadValidateButton());

        if (isValidatePressed && !wasValidatePressed) {
            // Simulate a mouse click at gamepad cursor position
            sf::Vector2i clickPos(static_cast<int>(_gamepadCursorPos.x), static_cast<int>(_gamepadCursorPos.y));
            handleMouseClick(clickPos);
        }

        wasValidatePressed = isValidatePressed;
    }

    void UISystem::renderGamepadCursor(sf::RenderWindow& window)
    {
        if (!_gamepadMode || !_settings.getGamepadEnabled())
            return;

        // Draw a circle cursor
        sf::CircleShape cursor(12.0f);
        cursor.setPosition({_gamepadCursorPos.x - 12.0f, _gamepadCursorPos.y - 12.0f});
        cursor.setFillColor(sf::Color(0, 0, 0, 0));
        cursor.setOutlineThickness(3.0f);
        cursor.setOutlineColor(sf::Color(255, 200, 100));

        // Inner dot
        sf::CircleShape dot(4.0f);
        dot.setPosition({_gamepadCursorPos.x - 4.0f, _gamepadCursorPos.y - 4.0f});
        dot.setFillColor(sf::Color(255, 200, 100));

        window.draw(cursor);
        window.draw(dot);
    }

} // namespace Client::Systems
