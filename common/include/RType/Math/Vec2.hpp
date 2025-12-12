/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Vec2.hpp, Vector of 2 class declaration
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
 * @file Vec2.hpp
 * @brief Declaration of the 2-dimensional vector class.
 * @author Robin Toillon
 */

#ifndef RTYPE_VEC2_HPP_
    #define RTYPE_VEC2_HPP_

    #include "RType/Assert.hpp"
    #include "RType/Math/details/VecBase.hpp"

namespace rtp
{
    /**
     * @brief A 2-dimensional vector.
     *
     * @tparam T Must be an integral or floating-point type.
     */
    template <Numeric T>
    struct Vec2 : details::VecBase<T, 2> {
        T x{}; /* X coordinate. */
        T y{}; /* Y coordinate. */

        /** @brief Default constructor. Initializes to (0, 0). */
        constexpr Vec2(void) = default;

        /** @brief Parameterized constructor. */
        constexpr Vec2(T x, T y) noexcept;

        /** @brief Default destructor. */
        constexpr ~Vec2() = default;

        /** @brief Checks if two vectors are exactly equal. */
        [[nodiscard]]
        constexpr bool operator==(const Vec2 &) const noexcept = default;

        /**
         * @brief Accesses the coordinate at the specified index.
         * 
         * @param index The index (0 for x, 1 for y).
         * @return Reference to the coordinate.
         */
        [[nodiscard]]
        constexpr auto &operator[](this auto &self, size_t index) noexcept;
    };

    using Vec2f = Vec2<float>;
    using Vec2d = Vec2<double>;
    using Vec2i = Vec2<int>;
    using Vec2u = Vec2<unsigned int>;
}

    #include "Vec2.tpp"

#endif /* !RTYPE_VEC2_HPP_ */
