/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Vec3.hpp, Vector of 3 class declaration
*/

/*
** MIT License
**
** Copyright (c) 2025 Robin Toillon
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * @file Vec3.hpp
 * @brief Declaration of the 3-dimensional vector class.
 * @author Robin Toillon
 */

#ifndef RTYPE_VEC3_HPP_
    #define RTYPE_VEC3_HPP_

    #include "RType/Assert.hpp"
    #include "RType/Math/details/VecBase.hpp"

namespace rtp
{
    /**
     * @brief A 3-dimensional vector.
     *
     * @tparam T Must be an integral or floating-point type.
     */
    template <Numeric T>
    struct Vec3 : details::VecBase<T, 3> {
        T x{}; /**< X coordinate. */
        T y{}; /**< Y coordinate. */
        T z{}; /**< Z coordinate. */

        /** @brief Default constructor. Initializes to (0, 0, 0). */
        constexpr Vec3(void) = default;

        /** @brief Parameterized constructor. */
        constexpr Vec3(T x, T y, T z) noexcept;

        /** @brief Default destructor. */
        constexpr ~Vec3() = default;

        /** @brief Checks if two vectors are exactly equal. */
        [[nodiscard]]
        constexpr bool operator==(const Vec3 &) const noexcept = default;

        /**
         * @brief Accesses the coordinate at the specified index.
         * 
         * @param index The index (0 for x, 1 for y, 2 for z).
         * @return Reference to the coordinate.
         */
        [[nodiscard]]
        constexpr auto &operator[](this auto &self, size_t index) noexcept;

        /**
         * @brief The cross product of two 3D vectors.
         */
        [[nodiscard]]
        constexpr auto cross(const Vec3 &other) const noexcept
            -> Vec3;
    };

    using Vec3f = Vec3<float>;
    using Vec3d = Vec3<double>;
    using Vec3i = Vec3<int>;
    using Vec3u = Vec3<unsigned int>;
}

    #include "Vec3.tpp"

#endif /* !RTYPE_VEC3_HPP_ */
