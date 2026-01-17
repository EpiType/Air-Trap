/**
 * File   : Event.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_INPUT_EVENT_HPP_
    #define ENGINE_INPUT_EVENT_HPP_

/**
 * @namespace engine::input
 * @brief Input handling for the engine
 */
namespace engine::input
{
    /**
     * @enum KeyCode
     * @brief Enumeration of keyboard key codes
     */
    enum class KeyCode {
        Unknown = 0,
        Up,
        Down,
        Left,
        Right,
        Escape,
        Return,
        Space,
        F1,
        A, B, C, D, E, F, G, H, I, J, K, L, M,
        N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
        Num0, Num1, Num2, Num3, Num4, Num5, Num6, Num7, Num8, Num9
    };

    /**
     * @enum MouseButton
     * @brief Enumeration of mouse buttons
     */
    enum class MouseButton {
        Left = 0,
        Right,
        Middle,
        Unknown
    };

    /**
     * @enum EventType
     * @brief Enumeration of input event types
     */
    enum class EventType {
        None,
        Close,
        KeyDown,
        KeyUp,
        TextEntered,
        MouseMove,
        MouseButtonDown,
        MouseButtonUp,
        MouseWheel
    };

    /**
     * @struct Event
     * @brief Structure representing an input event
     */
    struct Event {
        EventType type{EventType::None};
        KeyCode key{KeyCode::Unknown};
        MouseButton mouseButton{MouseButton::Unknown};
        int x{0};
        int y{0};
        float wheelDelta{0.0f};
        char32_t text{0};
    };
}

#endif /* !ENGINE_INPUT_EVENT_HPP_ */
