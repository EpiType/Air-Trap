/**
 * File   : vec2f.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_MATH_VEC2F_HPP_
    #define RTYPE_MATH_VEC2F_HPP_

/**
 * @namespace rtp::math
 * @brief Mathematical utilities for R-Type protocol
 */
namespace rtp::math {

    /**
     * @struct vec2f
     * @brief 2D vector with float components
     */
    struct Vec2f {
        float x; /**< X component */
        float y; /**< Y component */

        /**
         * @brief Default constructor initializes vector to (0.0f, 0.0f)
         */
        vec2f() : x(0.0f), y(0.0f) {}

        /**
         * @brief Parameterized constructor
         * @param x X component
         * @param y Y component
         */
        vec2f(float x, float y) : x(x), y(y) {}
    };

}  // namespace rtp::math

#endif /* !RTYPE_MATH_VEC2F_HPP_ */