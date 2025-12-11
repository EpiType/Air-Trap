/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Vec4.hpp, Vector of 4 class declaration
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
 * @file Vec4.hpp
 * @brief Declaration of the 4-dimensional vector class.
 * @author Robin Toillon
 */

#ifndef RTYPE_VEC4_HPP_
    #define RTYPE_VEC4_HPP_

    #include "RType/Assert.hpp"
    #include "RType/Math/details/VecBase.hpp"

namespace rtp
{
    /**
     * @brief A 4-dimensional vector.
     *
     * @tparam T Must be an integral or floating-point type.
     */
    template <Numeric T>
    struct Vec4 : details::VecBase<T, 4> {
        T x{}; /**< X coordinate. */
        T y{}; /**< Y coordinate. */
        T z{}; /**< Z coordinate. */
        T w{}; /**< W coordinate. */

        /** @brief Default constructor. Initializes to (0, 0, 0, 0). */
        constexpr Vec4(void) = default;

        /** @brief Parameterized constructor. */
        constexpr Vec4(T x, T y, T z, T w) noexcept;

        /** @brief Default destructor. */
        constexpr ~Vec4() = default;

        /** @brief Checks if two vectors are exactly equal. */
        [[nodiscard]]
        constexpr bool operator==(const Vec4 &) const noexcept = default;

        /**
         * @brief Accesses the coordinate at the specified index.
         * 
         * @param index The index (0 for x, 1 for y, 2 for z, 3 for w).
         * @return Reference to the coordinate.
         */
        [[nodiscard]]
        constexpr auto &operator[](this auto &self, size_t index) noexcept;
    };

    using Vec4f = Vec4<float>;
    using Vec4d = Vec4<double>;
    using Vec4i = Vec4<int>;
    using Vec4u = Vec4<unsigned int>;
}

    #include "Vec4.tpp"

#endif /* !RTYPE_VEC4_HPP_ */
