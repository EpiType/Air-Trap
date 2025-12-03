/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Rect
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Rect
 * @brief Component representing a rectangle area.
 * Used for defining boundaries, hitboxes, or UI elements.
 */
struct Rect {
    float x{0.0f};      /**< X position of the rectangle */
    float y{0.0f};      /**< Y position of the rectangle */
    float width{0.0f};  /**< Width of the rectangle */
    float height{0.0f}; /**< Height of the rectangle */
};
}  // namespace rtp::ecs::components