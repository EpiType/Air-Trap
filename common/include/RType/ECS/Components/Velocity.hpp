/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Velocity
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Velocity
 * @brief Component representing a 2D velocity.
 */
struct Velocity {
    float vx{0.0f}; /**< Velocity in the X direction */
    float vy{0.0f}; /**< Velocity in the Y direction */
};
}  // namespace rtp::ecs::components