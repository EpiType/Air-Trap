/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Damage
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Damage
 * @brief Component representing damage value.
 */
struct Damage {
    int amount{0}; /**< Amount of damage */
};
}  // namespace rtp::ecs::components