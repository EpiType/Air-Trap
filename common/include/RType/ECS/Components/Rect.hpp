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
 * Used for defining bounding boxes or collision areas.
 */
struct BoundingBox {
    float width{0.0f};  /**< Width of the rectangle */
    float height{0.0f}; /**< Height of the rectangle */
};
}  // namespace rtp::ecs::components