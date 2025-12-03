/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Weapon
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct Weapon
 * @brief Component representing a weapon.
 */
struct Weapon {
    float fireRate{1.0f};     /**< Shots per second */
    float lastShotTime{0.0f}; /**< Time since last shot */
    int damage{10};           /**< Damage per shot */
};
}  // namespace rtp::ecs::components