/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** MenuSystem
 * implementation
*/

#include "Systems/MenuSystem.hpp"

#include "RType/Logger.hpp"

namespace Client::Systems
{
    void MenuSystem::update(float dt)
    {
        (void)dt;

        auto buttonsCheck =
            _registry.getComponents<rtp::ecs::components::ui::Button>();
        if (!buttonsCheck) {
            return;
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(_window);

        handleMouseMove(mousePos);

        buttonsCheck =
            _registry.getComponents<rtp::ecs::components::ui::Button>();
        if (!buttonsCheck) {
            return;
        }

        static bool wasPressed = false;
        bool isPressed = sf::Mouse::isButtonPressed(sf::Mouse::Button::Left);

        if (isPressed && !wasPressed) {
            handleMouseClick(mousePos);
        }

        wasPressed = isPressed;
    }

    void MenuSystem::handleEvent(const sf::Event &event)
    {
        if (const auto *te = event.getIf<sf::Event::TextEntered>()) {
            handleTextEntered(te->unicode);
        }
        if (const auto *kp = event.getIf<sf::Event::KeyPressed>()) {
            handleKeyPressed(kp->code);
        }
    }

    void MenuSystem::handleMouseMove(const sf::Vector2i &mousePos)
    {
        auto buttonsResult =
            _registry.getComponents<rtp::ecs::components::ui::Button>();
        if (!buttonsResult)
            return;

        auto &buttons = buttonsResult.value().get();
        for (const auto &entity : buttons.getEntities()) {
            auto &button = buttons[entity];

            if (isMouseOverButton(button, mousePos)) {
                button.state = rtp::ecs::components::ui::ButtonState::Hovered;
            } else {
                button.state = rtp::ecs::components::ui::ButtonState::Idle;
            }
        }
    }

    void MenuSystem::handleMouseClick(const sf::Vector2i &mousePos)
    {
        auto dropdownsResult =
            _registry.getComponents<rtp::ecs::components::ui::Dropdown>();
        if (dropdownsResult) {
            auto &dropdowns = dropdownsResult.value().get();

            for (const auto &entity : dropdowns.getEntities()) {
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

            for (const auto &entity : dropdowns.getEntities()) {
                auto &dropdown = dropdowns[entity];

                if (isMouseOverDropdown(dropdown, mousePos)) {
                    dropdown.isOpen = !dropdown.isOpen;
                    rtp::log::info("Dropdown toggled: {}",
                                   dropdown.isOpen ? "open" : "closed");

                    for (const auto &otherEntity : dropdowns.getEntities()) {
                        if (otherEntity != entity) {
                            dropdowns[otherEntity].isOpen = false;
                        }
                    }
                    return;
                }
            }

            bool hadOpenDropdown = false;
            for (const auto &entity : dropdowns.getEntities()) {
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

                for (const auto &entity : inputs.getEntities()) {
                    if (isMouseOverTextInput(inputs[entity])) {
                        clickedOnAnyInput = true;
                        break;
                    }
                }

                if (clickedOnAnyInput) {
                    for (const auto &entity : inputs.getEntities()) {
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
                    for (const auto &entity : inputs.getEntities()) {
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
            for (const auto &entity : buttons.getEntities()) {
                auto &button = buttons[entity];
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
            for (const auto &entity : sliders.getEntities()) {
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

    bool MenuSystem::isMouseOverButton(
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

    bool MenuSystem::isMouseOverSlider(
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

    bool MenuSystem::isMouseOverDropdown(
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

    void MenuSystem::updateSliderValue(rtp::ecs::components::ui::Slider &slider,
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

    int MenuSystem::getDropdownOptionAtMouse(
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

    bool MenuSystem::isMouseOverTextInput(
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

    void MenuSystem::clearAllTextInputFocus()
    {
        auto inputsResult =
            _registry.getComponents<rtp::ecs::components::ui::TextInput>();
        if (!inputsResult)
            return;

        auto &inputs = inputsResult.value().get();
        for (const auto &e : inputs.getEntities()) {
            inputs[e].isFocused = false;
            inputs[e].showCursor = false;
            inputs[e].blinkTimer = 0.0f;
        }
    }

    void MenuSystem::focusTextInputAt(const sf::Vector2i &mousePos)
    {
        auto inputsResult =
            _registry.getComponents<rtp::ecs::components::ui::TextInput>();
        if (!inputsResult)
            return;

        auto &inputs = inputsResult.value().get();

        bool focusedOne = false;
        for (const auto &e : inputs.getEntities()) {
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

    void MenuSystem::handleTextEntered(std::uint32_t unicode)
    {
        auto inputsResult =
            _registry.getComponents<rtp::ecs::components::ui::TextInput>();
        if (!inputsResult)
            return;

        auto &inputs = inputsResult.value().get();

        if (unicode == 8) {
            for (const auto &e : inputs.getEntities()) {
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
            for (const auto &e : inputs.getEntities()) {
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

        for (const auto &e : inputs.getEntities()) {
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

    void MenuSystem::handleKeyPressed(sf::Keyboard::Key key)
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
            for (const auto &e : inputs.getEntities()) {
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
            for (const auto &e : inputs.getEntities()) {
                auto &in = inputs[e];
                if (!in.isFocused)
                    continue;

                if (in.onSubmit)
                    in.onSubmit(in.value);
                return;
            }
        }
    }

} // namespace Client::Systems
