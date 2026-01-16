/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Animation
*/

#ifndef ENGINE_ECS_COMPONENTS_ANIMATION_HPP_
    #define ENGINE_ECS_COMPONENTS_ANIMATION_HPP_

/**
 * @namespace engine::ecs::components
 * @brief ECS components for R-Type engine
 */
namespace engine::ecs::components {
    /**
     * @struct Animation
     * @brief Component representing an animation.
     */
    struct Animation {
        int totalFrames{1};         /**< Total frames including all animations */
        int currentFrame{0};        /**< Index of the currently displayed frame (0 to totalFrames - 1) */
        float frameDuration{0.1f};  /**< Duration of each frame in seconds */
        bool loop{true};            /**< Whether the animation should loop */
        float elapsedTime{0.0f};    /**< Time elapsed since the animation started */
        int frameLeft{0};           /**< Left position of the current frame */
        int frameTop{0};            /**< Top position of the current frame */
        int frameWidth{0};          /**< Width of the current frame */
        int frameHeight{0};         /**< Height of the current frame */
    };
}  // namespace engine::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_ANIMATION_HPP_ */