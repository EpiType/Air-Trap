/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Position
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Position
 * @brief Component representing a 2D position.
 */
struct Position {
    float x{0.0f}; /**< X coordinate */
    float y{0.0f}; /**< Y coordinate */
};
}  // namespace rtp::ecs::components