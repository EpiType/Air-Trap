/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Button
*/

#pragma once

#include <functional>
#include <string>
#include "RType/Math/Vec2.hpp"

namespace rtp::ecs::components::ui {

/**
 * @enum ButtonState
 * @brief State of a button (idle, hovered, pressed)
 */
enum class ButtonState {
    Idle,
    Hovered,
    Pressed
};

/**
 * @struct Button
 * @brief Component representing a clickable button
 */
struct Button {
    std::string text;                     /**< Text displayed on the button */
    Vec2f position;                       /**< Position of the button */
    Vec2f size;                           /**< Size of the button */
    ButtonState state{ButtonState::Idle}; /**< Current state */
    std::function<void()> onClick;        /**< Callback when clicked */
    
    // Visual properties
    uint8_t idleColor[3]{100, 100, 100};     /**< RGB color when idle */
    uint8_t hoverColor[3]{150, 150, 150};    /**< RGB color when hovered */
    uint8_t pressedColor[3]{50, 50, 50};     /**< RGB color when pressed */
};

}  // namespace rtp::ecs::components::ui