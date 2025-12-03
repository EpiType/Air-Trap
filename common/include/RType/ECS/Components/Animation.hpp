/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Animation
*/

#pragma once

#include <SFML/Graphics/Rect.hpp>

namespace rtp::ecs::components {
/**
 * @struct Animation
 * @brief Component representing an animation.
 */
struct Animation {
    int frameCount{1};         /**< Total number of frames in the animation */
    int totalFrames{1};        /**< Total frames including all animations */
    float frameDuration{0.1f}; /**< Duration of each frame in seconds */
    bool loop{true};           /**< Whether the animation should loop */
    float elapsedTime{0.0f};   /**< Time elapsed since the animation started */
    sf::IntRect frameRect;     /**< Rectangle defining the current frame area */
};
}  // namespace rtp::ecs::components