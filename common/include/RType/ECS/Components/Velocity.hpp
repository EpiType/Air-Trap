/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Velocity
*/

#pragma once

#include "RType/Math/Vec2.hpp"

namespace rtp::ecs::components {
/**
 * @struct Velocity
 * @brief Component representing a 2D velocity.
 */
struct Velocity {
    Vec2f direction{0.0f, 0.0f};    /**< Velocity in X and Y directions */
    float speed{0.0f};              /**< Speed multiplier */
};
}  // namespace rtp::ecs::components