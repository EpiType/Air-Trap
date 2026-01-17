/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Powerup
*/

#pragma once
#include <cstdint>

namespace rtp::ecs::components {
/**
 * @enum PowerupType
 * @brief Supported powerup types.
 */
enum class PowerupType : uint8_t {
    Heal = 1,         // Red - Restore 1 heart
    Speed = 2,        // (unused)
    DoubleFire = 3,   // Yellow/White - Double fire for 20s
    Shield = 4        // Green - Absorb 1 hit
};

/**
 * @struct Powerup
 * @brief Component representing a powerup pickup.
 */
struct Powerup {
    PowerupType type{PowerupType::Heal}; /**< Powerup type */
    float value{0.0f};                  /**< Heal amount or effect strength */
    float duration{0.0f};               /**< Duration for timed effects (0 = permanent) */
};
}  // namespace rtp::ecs::components
