/**
 * File   : Transform.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef ENGINE_ECS_COMPONENTS_TRANSFORM_HPP_
    #define ENGINE_ECS_COMPONENTS_TRANSFORM_HPP_

        #include "engine/math/Vec2.hpp"

/**
 * @namespace aer::ecs::components
 * @brief ECS components for R-Type protocol
 */
namespace aer::ecs::components
{
    /**
     * @struct Transform
     * @brief Component representing position, rotation, and scale of an entity.
     */
    struct Transform {
        aer::math::Vec2f position{0.0f, 0.0f};   /**< X and Y coordinates */
        aer::math::Vec2f scale{1.0f, 1.0f};      /**< Scale factors for X and Y axes */
        float rotation{0.0f};                       /**< Rotation in degrees */
    };
} // namespace aer::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_TRANSFORM_HPP_ */
