/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Button
*/

#ifndef ENGINE_ECS_COMPONENTS_BUTTON_HPP_
    #define ENGINE_ECS_COMPONENTS_BUTTON_HPP_

    #include <functional>
    #include <string>
    #include "engine/Math/Vec2.hpp"
    #include "engine/ui/UiUtilities.hpp"

namespace engine::ecs::components
{
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
        ui::textContainer text;                 /**< Text displayed on the button */
        math::Vec2f position;                   /**< Position of the button */
        math::Vec2f size;                       /**< Size of the button */
        ButtonState state{ButtonState::Idle};   /**< Current state */
        std::function<void()> onClick;          /**< Callback when clicked */
        
        ui::color idleColor{100, 100, 100};     /**< RGB color when idle */
        ui::color idleColor{150, 150, 150};     /**< RGB color when hovered */
        ui::color pressedColor{50, 50, 50};     /**< RGB color when pressed */
    };
}  // namespace engine::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_BUTTON_HPP_ */