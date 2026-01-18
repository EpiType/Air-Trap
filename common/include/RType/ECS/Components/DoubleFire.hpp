/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** DoubleFire component
*/

#pragma once

namespace rtp::ecs::components {

/**
 * @struct DoubleFire
 * @brief Component for double fire powerup
 */
struct DoubleFire {
    float remainingTime{20.0f}; /**< Time remaining for double fire effect */
};

}  // namespace rtp::ecs::components
