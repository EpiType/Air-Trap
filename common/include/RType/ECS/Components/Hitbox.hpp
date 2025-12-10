/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Hitbox
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

/**
 * @enum CollisionShape
 * @brief Enum representing different shapes of hitboxes.
 */
enum class CollisionShape {
    Rectangle,
    Circle
};

/**
 * @struct Hitbox
 * @brief Component representing a rectangular hitbox.
 */
struct Hitbox {
    float width{0.0f};   /**< Width of the hitbox */
    float height{0.0f};  /**< Height of the hitbox */
    float radius{0.0f};  /**< Radius for circular hitboxes */
    float offsetX{0.0f}; /**< X offset from the entity's position */
    float offsetY{0.0f}; /**< Y offset from the entity's position */
    CollisionShape shape{CollisionShape::Rectangle}; /**< Shape of the hitbox */
    CollisionType collisionType{CollisionType::Solid}; /**< Type of collision behavior */
};
}  // namespace rtp::ecs::components