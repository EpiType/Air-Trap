/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** TextInput
*/

#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include "RType/Math/Vec2.hpp"

namespace rtp::ecs::components::ui {

struct TextInput {
    Vec2f position;
    Vec2f size{320.0f, 45.0f};

    std::string value;
    std::string placeholder{"..."};
    std::string fontPath{"assets/fonts/main.ttf"};
    unsigned int fontSize{22};

    bool isFocused{false};
    bool isPassword{false};
    std::size_t maxLength{64};

    bool showCursor{true};
    float blinkTimer{0.0f};
    float blinkInterval{0.5f};

    std::function<void(const std::string&)> onSubmit;
    std::function<void(const std::string&)> onChange;

    uint8_t bgColor[3]{30, 30, 30};
    uint8_t borderColor[3]{120, 120, 120};
    uint8_t focusBorderColor[3]{100, 200, 100};
    uint8_t textColor[3]{255, 255, 255};
    uint8_t placeholderColor[3]{150, 150, 150};
    uint8_t alpha{255};
    int zIndex{5};

    std::string getDisplayValue() const {
        if (!isPassword) return value;
        return std::string(value.size(), '*');
    }
};

} // namespace rtp::ecs::components::ui
