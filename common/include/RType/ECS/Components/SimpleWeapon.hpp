/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Weapon
*/

#pragma once

#include <cstdint>

namespace rtp::ecs::components {

/**
 * @enum WeaponKind
 * @brief Types of player weapons
 */
enum class WeaponKind : uint8_t {
    Classic = 0,   /**< Default spam/charge laser */
    Beam = 1,      /**< Continuous beam 5s active, 5s cooldown */
    Paddle = 2,    /**< Melee reflector in front of ship */
    Tracker = 3,   /**< Weak auto-homing shots */
    Boomerang = 4  /**< Single projectile that returns */
};

/**
 * @struct SimpleWeapon
 * @brief Component representing a weapon configuration
 */
struct SimpleWeapon {
    WeaponKind kind{WeaponKind::Classic}; /**< Weapon type */
    float fireRate{0.3f};     /**< Shots per second (or cooldown) */
    float lastShotTime{0.0f}; /**< Time since last shot */
    int damage{10};           /**< Damage per shot */
    
    // Ammo system
    int ammo{-1};             /**< Current ammo (-1 = infinite) */
    int maxAmmo{-1};          /**< Max ammo (-1 = infinite) */
    
    // Special properties (interpreted per kind)
    float beamDuration{0.0f};    /**< Beam active time (Beam) */
    float beamCooldown{0.0f};    /**< Beam cooldown time (Beam) */
    float beamActiveTime{0.0f};  /**< Current beam activity timer */
    bool beamActive{false};      /**< Is beam currently firing */
    float beamCooldownRemaining{0.0f}; /**< Remaining cooldown for beam (runtime) */
    
    int difficulty{2};           /**< Difficulty to use this weapon (1-5) */
    
    bool canReflect{false};      /**< Can reflect enemy bullets (Paddle) */
    bool homing{false};          /**< Shots home to enemies (Tracker) */
    bool isBoomerang{false};     /**< Projectile returns (Boomerang) */
    bool boomerangOut{false};    /**< Boomerang currently flying */
};

}  // namespace rtp::ecs::components