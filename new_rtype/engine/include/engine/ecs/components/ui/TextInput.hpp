/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** TextInput
*/

#ifndef ENGINE_ECS_COMPONENTS_TEXTINPUT_HPP_
    #define ENGINE_ECS_COMPONENTS_TEXTINPUT_HPP_

    #include <cstdint>
    #include <functional>
    #include <string>
    #include "engine/math/Vec2.hpp"
    #include "engine/ui/UiUtilities.hpp"

namespace aer::ecs::components
{
    struct TextInput {
        math::Vec2f position;
        math::Vec2f size{320.0f, 45.0f};

        std::string value;
        ui::textContainer placeholder;

        bool isFocused{false};
        bool isPassword{false};
        std::size_t maxLength{64};

        bool showCursor{true};
        float blinkTimer{0.0f};
        float blinkInterval{0.5f};

        std::function<void(const std::string&)> onSubmit;
        std::function<void(const std::string&)> onChange;

        ui::color bgColor{30, 30, 30};
        ui::color borderColor{120, 120, 120};
        ui::color focusBorderColor{100, 200, 100};
        ui::color textColor{255, 255, 255};
        ui::color placeholderColor{150, 150, 150};
        uint8_t alpha{255};
        int zIndex{5};

        std::string getDisplayValue() const {
            if (!isPassword) return value;
            return std::string(value.size(), '*');
        }
    };
} // namespace aer::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_TEXTINPUT_HPP_ */
