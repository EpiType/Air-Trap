/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Parralax
*/

#ifndef RTYPE_GAMES_RTYPE_SRC_SHARED_COMPONENTS_PARALLAXLAYER_HPP_
    #define RTYPE_GAMES_RTYPE_SRC_SHARED_COMPONENTS_PARALLAXLAYER_HPP_

namespace rtp::ecs::components
{
    /**
     * @struct ParallaxLayer
     * @brief Tag component indicating that an entity should loop its position 
     * when it exits the screen (infinite scrolling).
     */
    struct ParallaxLayer {
        float textureWidth = {0.0f}; /**< Width of the texture used for parallax scrolling */
        float speed = {100.0f};      /**< Scrolling speed of the parallax layer */
    };

} // namespace rtp::ecs::components

#endif /* !RTYPE_GAMES_RTYPE_SRC_SHARED_COMPONENTS_PARALLAXLAYER_HPP_ */