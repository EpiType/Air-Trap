/**
 * File   : Vec4.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_MATH_VEC4_HPP_
    #define RTYPE_MATH_VEC4_HPP_

    #include <cmath>
    #include <type_traits>
    #include <concepts>

namespace engine::math {

    /**
     * @brief 4D vector class template
     * @tparam T Numeric type (e.g., float, double, int)
     * @details Represents a 4D vector and provides common vector operations.
     */
    template <std::floating_point T>
    struct Vec4 {
        T x{0}; /**< X component */
        T y{0}; /**< Y component */
        T z{0}; /**< Z component */
        T w{0}; /**< W component */

        Vec4 operator+(const Vec4& other) const {
            return Vec4{x + other.x, y + other.y, z + other.z, w + other.w};
        }

        Vec4 operator-(const Vec4& other) const {
            return Vec4{x - other.x, y - other.y, z - other.z, w - other.w};
        }

        Vec4 operator*(T scalar) const {
            return Vec4{x * scalar, y * scalar, z * scalar, w * scalar};
        }

        Vec4 operator/(T scalar) const {
            return Vec4{x / scalar, y / scalar, z / scalar, w / scalar};
        }

        T dot(const Vec4& other) const {
            return x * other.x + y * other.y + z * other.z + w * other.w;
        }

        T magnitude() const {
            return std::sqrt(x * x + y * y + z * z + w * w);
        }

        Vec4 normalize() const {
            T mag = magnitude();
            return mag == 0 ? Vec4{0, 0, 0, 0} : (*this) / mag;
        }
    };

    using Vec4f = Vec4<float>;   /**< 4D vector with float components */
    using Vec4d = Vec4<double>;  /**< 4D vector with double components */

} // namespace engine::math

#endif /* !RTYPE_MATH_VEC4_HPP_ */