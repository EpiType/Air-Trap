/**
 * File   : Vec3.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_MATH_VEC3_HPP_
    #define RTYPE_MATH_VEC3_HPP_

    #include <cmath>
    #include <type_traits>
    #include <concepts>

namespace aer::math {

    /**
     * @brief 3D vector class template
     * @tparam T Numeric type (e.g., float, double, int)
     * @details Represents a 3D vector and provides common vector operations.
     */
    template <std::floating_point T>
    struct Vec3 {
        T x{0}; /**< X component */
        T y{0}; /**< Y component */
        T z{0}; /**< Z component */

        Vec3 operator+(const Vec3& other) const {
            return Vec3{x + other.x, y + other.y, z + other.z};
        }

        Vec3 operator-(const Vec3& other) const {
            return Vec3{x - other.x, y - other.y, z - other.z};
        }

        Vec3 operator*(T scalar) const {
            return Vec3{x * scalar, y * scalar, z * scalar};
        }

        Vec3 operator/(T scalar) const {
            return Vec3{x / scalar, y / scalar, z / scalar};
        }

        T dot(const Vec3& other) const {
            return x * other.x + y * other.y + z * other.z;
        }

        T magnitude() const {
            return std::sqrt(x * x + y * y + z * z);
        }

        Vec3 normalize() const {
            T mag = magnitude();
            return mag == 0 ? Vec3{0, 0, 0} : (*this) / mag;
        }
    };

    using Vec3f = Vec3<float>;   /**< 3D vector with float components */
    using Vec3d = Vec3<double>;  /**< 3D vector with double components */

} // namespace aer::math

#endif /* !RTYPE_MATH_VEC3_HPP_ */