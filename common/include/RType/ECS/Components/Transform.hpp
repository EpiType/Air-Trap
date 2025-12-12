/**
 * File   : Transform.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ECS_COMPONENTS_TRANSFORM_HPP_
#define RTYPE_ECS_COMPONENTS_TRANSFORM_HPP_

    #include "RType/Math/Vec2.hpp"

/**
 * @namespace rtp::ecs::components
 * @brief ECS components for R-Type protocol
 */
namespace rtp::ecs::components {

    /**
     * @struct Transform
     * @brief Component representing position, rotation, and scale of an entity.
     */
    struct Transform {
        Vec2f position{0.0f, 0.0f}; /**< X and Y coordinates */
        float rotation{0.0f};          /**< Rotation in degrees */
        Vec2f scale{1.0f, 1.0f}; /**< Scale factors for X and Y axes */
    };

} // namespace rtp::ecs::components

#endif /* !RTYPE_ECS_COMPONENTS_TRANSFORM_HPP_ */
