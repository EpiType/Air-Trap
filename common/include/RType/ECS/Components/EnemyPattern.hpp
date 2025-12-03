/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** EnemyPattern
*/

#pragma once

namespace rtp::ecs::components {

/**
 * @enum EnemyPatterns
 * @brief Enum representing different enemy movement patterns.
 */
enum class EnemyPatterns {
    StraightLine = 0, /**< Moves in a straight line */
    ZigZag = 1,       /**< Moves in a zigzag pattern */
    Circular = 2,     /**< Moves in a circular pattern */
    SineWave = 3,     /**< Moves in a sine wave pattern */
    Kamikaze = 4,     /**< Moves directly towards the player */
    Static = 5        /**< Remains stationary */
};

/**
 * @struct EnemyPattern
 * @brief Component representing an enemy movement pattern.
 */
struct EnemyPattern {
    int patternId{0};  /**< Identifier for the movement pattern */
    float speed{0.0f}; /**< Speed of the enemy following the pattern */
};
}  // namespace rtp::ecs::components