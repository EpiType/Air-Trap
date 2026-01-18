/**
 * File   : Vec2.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_MATH_VEC2_HPP_
    #define RTYPE_MATH_VEC2_HPP_

    #include <cmath>
    #include <type_traits>
    #include <concepts>

namespace aer::math {

    /**
     * @brief 2D vector class template
     * @tparam T Numeric type (e.g., float, double, int)
     * @details Represents a 2D vector and provides common vector operations.
     */
    template <std::floating_point T>
    struct Vec2 {
        T x{0}; /**< X component */
        T y{0}; /**< Y component */

        Vec2 operator+(const Vec2& other) const {
            return Vec2{x + other.x, y + other.y};
        }

        Vec2 operator-(const Vec2& other) const {
            return Vec2{x - other.x, y - other.y};
        }

        Vec2 operator*(T scalar) const {
            return Vec2{x * scalar, y * scalar};
        }

        Vec2 operator/(T scalar) const {
            return Vec2{x / scalar, y / scalar};
        }

        T dot(const Vec2& other) const {
            return x * other.x + y * other.y;
        }

        T magnitude() const {
            return std::sqrt(x * x + y * y);
        }

        Vec2 normalize() const {
            T mag = magnitude();
            return mag == 0 ? Vec2{0, 0} : (*this) / mag;
        }
    };

    using Vec2f = Vec2<float>;   /**< 2D vector with float components */
    using Vec2d = Vec2<double>;  /**< 2D vector with double components */

} // namespace aer::math

#endif /* !RTYPE_MATH_VEC2_HPP_ */