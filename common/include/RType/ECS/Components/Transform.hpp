/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Position
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Vector2f
 * @brief Simple 2D vector structure.
 */
struct Vector2f {
    float x, y;
    Vector2f(float x = 0.0f, float y = 0.0f) : x(x), y(y) {}
};

/**
 * @struct Position
 * @brief Component representing a 2D position.
 */
struct Transform {
    Vector2f position{0.0f, 0.0f}; /**< X and Y coordinates */
    float rotation{0.0f};          /**< Rotation in degrees */
    Vector2f scale{1.0f, 1.0f};    /**< Scale factors for X and Y axes */
};
}  // namespace rtp::ecs::components