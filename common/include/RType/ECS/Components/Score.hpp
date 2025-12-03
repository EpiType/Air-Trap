/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Score
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Score
 * @brief Component representing an entity's score.
 */
struct Score {
    int value{0}; /**< Current score value */
};
}  // namespace rtp::ecs::components