/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** ShieldVisual component - Visual effect for shield power-up
*/

#pragma once

namespace rtp::ecs::components {

/**
 * @brief Component for visual shield effect (green circle around player)
 */
struct ShieldVisual {
    float animationTime{0.0f}; ///< Time for animation effects (pulse, etc.)
    float alpha{200.0f};       ///< Transparency of the shield circle (0-255)
};

} // namespace rtp::ecs::components
