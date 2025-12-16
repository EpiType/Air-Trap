/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Vec4.tpp, Vector of 4 class implementation
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
 * @file Vec4.tpp
 * @brief Implementation of the 4-dimensional vector class.
 * @author Robin Toillon
 */

namespace rtp
{
    template<Numeric T>
    constexpr Vec4<T>::Vec4(T x_, T y_, T z_, T w_) noexcept
                           : x{x_}, y{y_}, z{z_}, w{w_}
    {
    }

    template <Numeric T>
    constexpr auto &Vec4<T>::operator[](this auto &self,
                                        std::size_t index) noexcept
    {
        switch (index) {
            case 0:
                return self.x;
            case 1:
                return self.y;
            case 2:
                return self.z;
            case 3:
                return self.w;
            default:
                RTP_ASSERT(false, "Vec4: Index {} out of bounds", index);
                std::unreachable();
        }
    }
}
