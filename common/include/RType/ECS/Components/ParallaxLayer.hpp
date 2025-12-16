/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Parralax
*/

#pragma once

namespace rtp::ecs::components {

/**
 * @struct ParallaxLayer
 * @brief Tag component indicating that an entity should loop its position 
 * when it exits the screen (infinite scrolling).
 */
struct ParallaxLayer {
    float textureWidth = {0.0f}; /**< Width of the texture used for parallax scrolling */

};

} // namespace rtp::ecs::components