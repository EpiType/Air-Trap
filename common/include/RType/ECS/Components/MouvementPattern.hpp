/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** EnemyPattern
*/

#pragma once

namespace rtp::ecs::components {

/**
 * @enum Patterns
 * @brief Enum representing different enemy movement patterns.
 */
enum class Patterns {
    StraightLine, /**< Moves in a straight line */
    ZigZag,       /**< Moves in a zigzag pattern */
    Circular,     /**< Moves in a circular pattern */
    SineWave,     /**< Moves in a sine wave pattern */
    Kamikaze,     /**< Moves directly towards the player */
    Static        /**< Remains stationary */
};

/**
 * @struct MouvementPattern
 * @brief Component representing a movement pattern.
 */
struct MouvementPattern {
    Patterns pattern{Patterns::StraightLine}; /**< The movement pattern type */
    float speed{0.0f}; /**< Speed of the following the pattern */
    float amplitude{0.0f}; /**< Amplitude for patterns like SineWave */
    float frequency{0.0f}; /**< Frequency for patterns like SineWave */
};
}  // namespace rtp::ecs::components