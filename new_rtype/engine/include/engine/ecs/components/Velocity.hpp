/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Velocity
*/

#ifndef ENGINE_ECS_COMPONENTS_VELOCITY_HPP_
    #define ENGINE_ECS_COMPONENTS_VELOCITY_HPP_

#include "engine/math/Vec2.hpp"

namespace aer::ecs::components {
    /**
     * @struct Velocity
     * @brief Component representing a 2D velocity.
     */
    struct Velocity {
        aer::math::Vec2f direction{0.0f, 0.0f};      /**< Velocity in X and Y directions */
        float speed{0.0f};                              /**< Speed multiplier */
    };
}  // namespace aer::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_VELOCITY_HPP_ */