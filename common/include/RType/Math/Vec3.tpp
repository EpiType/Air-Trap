/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Vec3.tpp, Vector of 3 class implementation
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
 * @file Vec3.tpp
 * @brief Implementation of the 3-dimensional vector class.
 * @author Robin Toillon
 */

namespace rtp
{
    template<Numeric T>
    constexpr Vec3<T>::Vec3(T x_, T y_, T z_) noexcept : x{x_}, y{y_}, z{z_}
    {
    }

    template <Numeric T>
    constexpr auto &Vec3<T>::operator[](this auto &self,
                                        std::size_t index) noexcept
    {
        if (index == 0)
            return self.x;
        if (index == 1)
            return self.y;
        if (index == 2)
            return self.z;
        RTP_ASSERT(false, "Vec3: Index {} out of bounds", index);
        std::unreachable();
    }

    template <Numeric T>
    constexpr auto Vec3<T>::cross(const Vec3 &other) const noexcept
        -> Vec3
    {
        return Vec3{y * other.z - z * other.y,
                    z * other.x - x * other.z,
                    x * other.y - y * other.x};
    }
}
