/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** VecBase.tpp, CRTP base vector class implementation
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
 * @file VecBase.tpp
 * @brief Implementation of the CRTP base vector class.
 * @author Robin Toillon
 */

namespace rtp::details
{
    ///////////////////////////////////////////////////////////////////////////
    // Math Operations
    ///////////////////////////////////////////////////////////////////////////

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto VecBase<T, N>::squaredDistance(this const Self &self,
        const std::remove_cvref_t<Self> &other) noexcept
        -> T
    {
        T sum = T{0};

        for (std::size_t i = 0; i < N; ++i) {
            T diff = self[i] - other[i];
            sum += diff * diff;
        }

        return sum;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto VecBase<T, N>::distance(this const Self &self,
        const std::remove_cvref_t<Self> &other) noexcept
        -> RealT
    {
        return static_cast<RealT>(std::sqrt(self.squaredDistance(other)));
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto VecBase<T, N>::squaredLength(this const Self &self) noexcept
        -> T
    {
        T sum = T{0};

        for (std::size_t i = 0; i < N; ++i)
            sum += self[i] * self[i];

        return sum;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto VecBase<T, N>::length(this const Self &self) noexcept
        -> RealT
    {
        return static_cast<RealT>(std::sqrt(self.squaredLength()));
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto VecBase<T, N>::normalize(this Self &self) noexcept
        -> Self &
        requires std::floating_point<T>
    {
        T len = self.length();

        if (len != T{0})
            for (std::size_t i = 0; i < N; ++i)
                self[i] /= len;

        return self;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto VecBase<T, N>::normalized(this const Self &self) noexcept
        -> std::remove_cvref_t<Self>
        requires std::floating_point<T>
    {
        std::remove_cvref_t<Self> copy = self;
        return copy.normalize();
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto VecBase<T, N>::dot(this const Self &self,
                                      const std::remove_cvref_t<Self>
                                      &other) noexcept
        -> T
    {
        T result = T{0};

        for (std::size_t i = 0; i < N; ++i)
            result += self[i] * other[i];

        return result;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto VecBase<T, N>::project(this const Self &self,
        const std::remove_cvref_t<Self> &other) noexcept
        -> std::remove_cvref_t<Self>
        requires std::floating_point<T>
    {
        using VecType = std::remove_cvref_t<Self>;

        T otherSquaredLength = other.squaredLength();
        if (otherSquaredLength == T{0})
            return VecType{};

        T scalar = self.dot(other) / otherSquaredLength;
        VecType result;
        for (std::size_t i = 0; i < N; ++i)
            result[i] = other[i] * scalar;

        return result;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Vector & Vector operations (Member Operators)
    ///////////////////////////////////////////////////////////////////////////

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto &VecBase<T, N>::operator+=(this Self &self,
                    const std::remove_cvref_t<Self> &other) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
            self[i] += other[i];
        return self;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto &VecBase<T, N>::operator-=(this Self &self,
                    const std::remove_cvref_t<Self> &other) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
            self[i] -= other[i];
        return self;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto &VecBase<T, N>::operator*=(this Self &self,
                    const std::remove_cvref_t<Self> &other) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
            self[i] *= other[i];
        return self;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto &VecBase<T, N>::operator/=(this Self &self,
                    const std::remove_cvref_t<Self> &other) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
            self[i] /= other[i];
        return self;
    }

    ///////////////////////////////////////////////////////////////////////////
    // Vector & Scalar operations (Member Operators)
    ///////////////////////////////////////////////////////////////////////////

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto &VecBase<T, N>::operator+=(this Self &self, T scalar) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
            self[i] += scalar;
        return self;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto &VecBase<T, N>::operator-=(this Self &self, T scalar) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
            self[i] -= scalar;
        return self;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto &VecBase<T, N>::operator*=(this Self &self, T scalar) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
            self[i] *= scalar;
        return self;
    }

    template <Numeric T, std::size_t N>
    template <typename Self>
    constexpr auto &VecBase<T, N>::operator/=(this Self &self, T scalar) noexcept
    {
        for (std::size_t i = 0; i < N; ++i)
            self[i] /= scalar;
        return self;
    }
}
