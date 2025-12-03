/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** CollisionType
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @enum CollisionType
 * @brief Enum representing different types of collision behaviors.
 */
enum class CollisionType {
    None,    /**< No collision */
    Solid,   /**< Solid object, blocks movement */
    Trigger, /**< Trigger object, activates events on contact */
    Damage,  /**< Damaging object, causes damage on contact */
    Heal     /**< Healing object, restores health on contact */
};
}  // namespace rtp::ecs::components