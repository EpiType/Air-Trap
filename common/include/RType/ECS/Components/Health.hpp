/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Health
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Health
 * @brief Component representing an entity's health.
 */
struct Health {
    int currentHealth{100}; /**< Current health value */
    int maxHealth{100};     /**< Maximum health value */
};
}  // namespace rtp::ecs::components