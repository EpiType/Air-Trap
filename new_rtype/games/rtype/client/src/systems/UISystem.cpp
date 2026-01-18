/**
 * File   : UISystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/UISystem.hpp"

namespace rtp::client::systems
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    UISystem::UISystem(aer::ecs::Registry& registry,
                       aer::render::IRenderer& renderer,
                       Settings& settings)
        : _registry(registry),
          _renderer(renderer),
          _settings(settings),
          _gamepadCursorPos(0.0f, 0.0f)
    {
    }

    void UISystem::update(float dt)
    {
        updateGamepadCursor(dt);
        handleGamepadInput();

        const auto &input = aer::input::Input::instance();
        const int mouseX = input.mouseX();
        const int mouseY = input.mouseY();

        aer::math::Vec2f cursorPos{
            static_cast<float>(mouseX),
            static_cast<float>(mouseY)
        };
        if (_gamepadMode) {
            cursorPos = _gamepadCursorPos;
        }

        if (_registry.getComponents<aer::ecs::components::Button>()) {
            handleMouseMove(cursorPos);
        }

        const bool isPressed =
            input.isMousePressed(aer::input::MouseButton::Left);
        if (isPressed) {
            handleMouseClick(cursorPos);
        }
        // updateTextInputs(dt, mouseX, mouseY, isPressed);
    }

    void UISystem::handleEvent(const aer::input::Event& event)
    {
        if (event.type == aer::input::EventType::TextEntered) {
            handleTextInput(static_cast<std::uint32_t>(event.text));
        } else if (event.type == aer::input::EventType::KeyDown) {
            handleKeyPressed(event.key);
        }
    }

    void UISystem::renderGamepadCursor(aer::render::IRenderer& renderer)
    {
        (void)renderer;
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    void UISystem::handleMouseMove(aer::math::Vec2f mousePos)
    {
        auto buttonsResult =
            _registry.getComponents<aer::ecs::components::Button>();
        if (!buttonsResult) {
            return;
        }

        auto &buttons = buttonsResult.value().get();
        for (const auto &entity : buttons.entities()) {
            auto &button = buttons[entity];
            if (isMouseOverButton(button, mousePos)) {
                button.state = aer::ecs::components::ButtonState::Hovered;
            } else {
                button.state = aer::ecs::components::ButtonState::Idle;
            }
        }
    }

    void UISystem::handleMouseClick(aer::math::Vec2f mousePos)
    {
        auto dropdownsResult =
            _registry.getComponents<aer::ecs::components::Dropdown>();
        if (dropdownsResult) {
            auto &dropdowns = dropdownsResult.value().get();

            for (const auto &entity : dropdowns.entities()) {
                auto &dropdown = dropdowns[entity];
                if (dropdown.isOpen) {
                    int optionIndex = getDropdownOptionAtMouse(dropdown, mousePos);
                    if (optionIndex >= 0) {
                        dropdown.selectedIndex = optionIndex;
                        dropdown.isOpen = false;
                        if (dropdown.onSelect) {
                            dropdown.onSelect(optionIndex);
                        }
                        return;
                    }
                }
            }

            for (const auto &entity : dropdowns.entities()) {
                auto &dropdown = dropdowns[entity];
                if (isMouseOverDropdown(dropdown, mousePos)) {
                    dropdown.isOpen = !dropdown.isOpen;
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

        auto inputsResult =
            _registry.getComponents<aer::ecs::components::TextInput>();
        if (inputsResult) {
            auto &inputs = inputsResult.value().get();
            bool clickedOnAnyInput = false;

            for (const auto &entity : inputs.entities()) {
                if (isMouseOverTextInput(inputs[entity], mousePos)) {
                    clickedOnAnyInput = true;
                    break;
                }
            }

            if (clickedOnAnyInput) {
                focusTextInputAt(mousePos);
                return;
            }
            clearAllTextInputFocus();
        }

        auto buttonsResult =
            _registry.getComponents<aer::ecs::components::Button>();
        if (buttonsResult) {
            auto &buttons = buttonsResult.value().get();
            std::function<void()> callback;

            for (const auto &entity : buttons.entities()) {
                auto &button = buttons[entity];
                if (!button.onClick && button.text.content.empty()) {
                    continue;
                }
                if (isMouseOverButton(button, mousePos)) {
                    button.state = aer::ecs::components::ButtonState::Pressed;
                    if (button.onClick) {
                        callback = button.onClick;
                    }
                    break;
                }
            }

            if (callback) {
                callback();
                return;
            }
        }

        auto slidersResult =
            _registry.getComponents<aer::ecs::components::Slider>();
        if (slidersResult) {
            auto &sliders = slidersResult.value().get();
            for (const auto &entity : sliders.entities()) {
                auto &slider = sliders[entity];
                if (isMouseOverSlider(slider, mousePos)) {
                    slider.isDragging = true;
                    updateSliderValue(slider, mousePos);
                    return;
                }
            }
        }
    }

    bool UISystem::isMouseOverButton(const aer::ecs::components::Button& button,
                                     const aer::math::Vec2f& mousePos)
    {
        return mousePos.x >= button.position.x &&
               mousePos.x <= button.position.x + button.size.x &&
               mousePos.y >= button.position.y &&
               mousePos.y <= button.position.y + button.size.y;
    }

    bool UISystem::isMouseOverSlider(const aer::ecs::components::Slider& slider,
                                     const aer::math::Vec2f& mousePos)
    {
        return mousePos.x >= slider.position.x &&
               mousePos.x <= slider.position.x + slider.size.x &&
               mousePos.y >= slider.position.y &&
               mousePos.y <= slider.position.y + slider.size.y;
    }

    bool UISystem::isMouseOverDropdown(const aer::ecs::components::Dropdown& dropdown,
                                       const aer::math::Vec2f& mousePos)
    {
        return mousePos.x >= dropdown.position.x &&
               mousePos.x <= dropdown.position.x + dropdown.size.x &&
               mousePos.y >= dropdown.position.y &&
               mousePos.y <= dropdown.position.y + dropdown.size.y;
    }

    bool UISystem::updateSliderValue(aer::ecs::components::Slider& slider,
                                     const aer::math::Vec2f& mousePos)
    {
        float relativeX = mousePos.x - slider.position.x;
        float normalized = relativeX / slider.size.x;
        if (normalized < 0.0f) {
            normalized = 0.0f;
        } else if (normalized > 1.0f) {
            normalized = 1.0f;
        }

        const float newValue =
            slider.minValue + normalized * (slider.maxValue - slider.minValue);
        if (newValue == slider.currentValue) {
            return false;
        }

        slider.currentValue = newValue;
        if (slider.onChange) {
            slider.onChange(slider.currentValue);
        }
        return true;
    }

    int UISystem::getDropdownOptionAtMouse(const aer::ecs::components::Dropdown& dropdown,
                                           const aer::math::Vec2f& mousePos)
    {
        float optionHeight = dropdown.size.y;
        float startY = dropdown.position.y + dropdown.size.y;

        for (std::size_t i = 0; i < dropdown.options.size(); ++i) {
            float optionY = startY + static_cast<float>(i) * optionHeight;

            if (mousePos.x >= dropdown.position.x &&
                mousePos.x <= dropdown.position.x + dropdown.size.x &&
                mousePos.y >= optionY &&
                mousePos.y <= optionY + optionHeight) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

    bool UISystem::isMouseOverTextInput(const aer::ecs::components::TextInput& input,
                                        const aer::math::Vec2f& mousePos) const
    {
        return mousePos.x >= input.position.x &&
               mousePos.x <= input.position.x + input.size.x &&
               mousePos.y >= input.position.y &&
               mousePos.y <= input.position.y + input.size.y;
    }

    void UISystem::clearAllTextInputFocus()
    {
        auto inputsResult =
            _registry.getComponents<aer::ecs::components::TextInput>();
        if (!inputsResult) {
            return;
        }

        auto &inputs = inputsResult.value().get();
        for (const auto &e : inputs.entities()) {
            inputs[e].isFocused = false;
            inputs[e].showCursor = false;
            inputs[e].blinkTimer = 0.0f;
        }
    }

    void UISystem::focusTextInputAt(const aer::math::Vec2f& mousePos)
    {
        auto inputsResult =
            _registry.getComponents<aer::ecs::components::TextInput>();
        if (!inputsResult) {
            return;
        }

        auto &inputs = inputsResult.value().get();
        aer::ecs::Entity focused = aer::ecs::NullEntity;
        int focusedZ = 0;
        bool found = false;

        for (const auto &e : inputs.entities()) {
            auto &in = inputs[e];
            if (isMouseOverTextInput(in, mousePos)) {
                if (!found || in.zIndex >= focusedZ) {
                    focused = e;
                    focusedZ = in.zIndex;
                    found = true;
                }
            }
        }

        for (const auto &e : inputs.entities()) {
            auto &in = inputs[e];
            const bool shouldFocus = found && e == focused;
            in.isFocused = shouldFocus;
            in.showCursor = shouldFocus;
            in.blinkTimer = 0.0f;
        }
    }

    void UISystem::handleTextInput(std::uint32_t unicode)
    {
        auto inputsResult =
            _registry.getComponents<aer::ecs::components::TextInput>();
        if (!inputsResult) {
            return;
        }

        auto &inputs = inputsResult.value().get();

        if (unicode == 8) {
            for (const auto &e : inputs.entities()) {
                auto &in = inputs[e];
                if (!in.isFocused) {
                    continue;
                }
                if (!in.value.empty()) {
                    in.value.pop_back();
                    if (in.onChange) {
                        in.onChange(in.value);
                    }
                }
                return;
            }
            return;
        }
        if (unicode == 13) {
            for (const auto &e : inputs.entities()) {
                auto &in = inputs[e];
                if (!in.isFocused) {
                    continue;
                }
                if (in.onSubmit) {
                    in.onSubmit(in.value);
                }
                return;
            }
            return;
        }
        if (unicode < 32 || unicode > 127) {
            return;
        }

        const char c = static_cast<char>(unicode);
        for (const auto &e : inputs.entities()) {
            auto &in = inputs[e];
            if (!in.isFocused) {
                continue;
            }
            if (in.value.size() >= in.maxLength) {
                return;
            }
            in.value.push_back(c);
            if (in.onChange) {
                in.onChange(in.value);
            }
            return;
        }
    }

    void UISystem::handleKeyPressed(aer::input::KeyCode key)
    {
        if (key == aer::input::KeyCode::Escape) {
            clearAllTextInputFocus();
            return;
        }

        if (key == aer::input::KeyCode::Return) {
            auto inputsResult =
                _registry.getComponents<aer::ecs::components::TextInput>();
            if (!inputsResult) {
                return;
            }
            auto &inputs = inputsResult.value().get();
            for (const auto &e : inputs.entities()) {
                auto &in = inputs[e];
                if (!in.isFocused) {
                    continue;
                }
                if (in.onSubmit) {
                    in.onSubmit(in.value);
                }
                return;
            }
        }
    }

    // bool UISystem::updateTextInputs(float dt, int, int, bool)
    // {
    //     auto inputsOpt =
    //         _registry.getComponents<aer::ecs::components::TextInput>();
    //     if (!inputsOpt) {
    //         return false;
    //     }
    //     auto &inputs = inputsOpt.value().get();

    //     aer::ecs::Entity focused = aer::ecs::NullEntity;
    //     for (const auto &e : inputs.entities()) {
    //         if (inputs[e].isFocused) {
    //             focused = e;
    //             break;
    //         }
    //     }
    //     if (focused.isNull()) {
    //         return false;
    //     }

    //     auto &focusedInput = inputs[focused];
    //     focusedInput.blinkTimer += dt;
    //     if (focusedInput.blinkTimer >= focusedInput.blinkInterval) {
    //         focusedInput.blinkTimer = 0.0f;
    //         focusedInput.showCursor = !focusedInput.showCursor;
    //     }

    //     const auto &events = aer::input::Input::instance().events();
    //     for (const auto &event : events) {
    //         if (event.type == aer::input::EventType::TextEntered) {
    //             handleTextInput(static_cast<std::uint32_t>(event.text));
    //         } else if (event.type == aer::input::EventType::KeyDown) {
    //             handleKeyPressed(event.key);
    //         }
    //     }
    //     return true;
    // }

    void UISystem::updateGamepadCursor(float dt)
    {
        (void)dt;
    }

    void UISystem::handleGamepadInput()
    {
    }

    // bool UISystem::isMouseOver(const aer::math::Vec2f& pos,
    //                            const aer::math::Vec2f& size,
    //                            int mouseX,
    //                            int mouseY) const
    // {
    //     return mouseX >= pos.x && mouseX <= pos.x + size.x &&
    //            mouseY >= pos.y && mouseY <= pos.y + size.y;
    // }
} // namespace rtp::client::systems
