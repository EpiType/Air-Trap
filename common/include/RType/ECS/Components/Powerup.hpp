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
    Heal = 1,
    Speed = 2
};

/**
 * @struct Powerup
 * @brief Component representing a powerup pickup.
 */
struct Powerup {
    PowerupType type{PowerupType::Heal}; /**< Powerup type */
    float value{0.0f};                  /**< Heal amount or speed multiplier */
    float duration{0.0f};               /**< Duration for timed effects */
};
}  // namespace rtp::ecs::components
