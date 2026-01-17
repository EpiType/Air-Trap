/**
 * File   : ShieldVisual.hpp
 * License: MIT
 * Author : Angel SEVERAN <angel.severan@epitech.eu>
 * Date   : 17/01/2026
 */

#pragma once

namespace rtp::ecs::components {

/**
 * @struct ShieldVisual
 * @brief Component for visual shield effect displayed around the player
 * 
 * This component is client-side only and manages the visual representation
 * of the shield power-up. It displays two green pulsing circles around the
 * player entity when active.
 * 
 * The shield visual is automatically removed when:
 * - An enemy bullet collides with the player (detected client-side)
 * - The server removes the Shield component (synchronized via network)
 */
struct ShieldVisual {
    float animationTime{0.0f}; ///< Time accumulator for pulse animation
    float alpha{200.0f};       ///< Current transparency (150-255, pulsing effect)
};

} // namespace rtp::ecs::components
