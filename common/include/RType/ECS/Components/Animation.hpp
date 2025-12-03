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
    int frameLeft{0};           /**< Left position of the current frame */
    int frameTop{0};            /**< Top position of the current frame */
    int frameWidth{0};         /**< Width of the current frame */
    int frameHeight{0};        /**< Height of the current frame */
};
}  // namespace rtp::ecs::components