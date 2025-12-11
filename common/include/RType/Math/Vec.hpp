/*
** EPITECH PROJECT, 2025
** 
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
 * @file Vec.hpp
 * @brief Smart alias that selects the optimized struct based on N.
 * @author Robin Toillon
 */

#ifndef RTYPE_VECTOR_HPP_
    #define RTYPE_VECTOR_HPP_

    #include "RType/Math/Vec2.hpp"
    #include "RType/Math/Vec3.hpp"
    #include "RType/Math/Vec4.hpp"
    #include "RType/Math/details/VecN.hpp"
    #include <type_traits>

namespace rtp
{
    /**
     * @brief Smart alias that selects the optimized struct based on N.
     */
    template <Numeric T, std::size_t N>
    using Vec = std::conditional_t<N == 2, Vec2<T>,
                std::conditional_t<N == 3, Vec3<T>,
                std::conditional_t<N == 4, Vec4<T>,
                details::VecN<T, N>>>>;
}

#endif /* !RTYPE_VECTOR_HPP_ */
