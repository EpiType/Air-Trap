/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Hitbox
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Hitbox
 * @brief Component representing a rectangular hitbox.
 */
struct Hitbox {
    float width{0.0f};   /**< Width of the hitbox */
    float height{0.0f};  /**< Height of the hitbox */
    float offsetX{0.0f}; /**< X offset from the entity's position */
    float offsetY{0.0f}; /**< Y offset from the entity's position */
};
}  // namespace rtp::ecs::components