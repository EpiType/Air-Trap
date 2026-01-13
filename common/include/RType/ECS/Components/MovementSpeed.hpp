/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** MovementSpeed
*/

#pragma once

namespace rtp::ecs::components {
/**
 * @struct MovementSpeed
 * @brief Component representing movement speed with temporary boosts.
 */
struct MovementSpeed {
    float baseSpeed{200.0f};     /**< Base movement speed */
    float multiplier{1.0f};      /**< Active speed multiplier */
    float boostRemaining{0.0f};  /**< Remaining boost duration in seconds */
};
}  // namespace rtp::ecs::components
