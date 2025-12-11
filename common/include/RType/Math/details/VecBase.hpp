/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** VecBase.hpp, CRTP base vector class declaration
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
 * @file VecBase.hpp
 * @brief Declaration of the CRTP base vector class.
 * @author Robin Toillon
 */

#ifndef RTYPE_VECBASE_HPP_
    #define RTYPE_VECBASE_HPP_

    #include "RType/Math/details/NumericConcept.hpp"

    #include <cmath>
    #include <iostream>

namespace rtp::details
{

    /**
     * @brief CRTP base class for vectors
     * @tparam T Numeric type
     * of the vector components
     * @tparam N Dimension of the vector
     * @details Provides common functionality for vectors of various
     * dimensions using the Curiously Recurring Template Pattern (CRTP).
     * Enables code reuse and static polymorphism for vector operations.
     */
    template <Numeric T, std::size_t N>
    struct VecBase {
        using RealT = std::conditional_t<std::is_integral_v<T>, double, T>;

        template <typename Self>
        [[nodiscard]]
        constexpr auto squaredDistance(this const Self &self,
            const std::remove_cvref_t<Self> &other) noexcept
            -> T;

        template <typename Self>
        [[nodiscard]]
        constexpr auto distance(this const Self &self,
            const std::remove_cvref_t<Self> &other) noexcept
            -> RealT;

        template <typename Self>
        [[nodiscard]]
        constexpr auto squaredLength(this const Self &self) noexcept -> T;

        template <typename Self>
        [[nodiscard]]
        constexpr auto length(this const Self &self) noexcept -> RealT;

        template <typename Self>
        [[nodiscard]]
        constexpr auto normalize(this Self &self) noexcept -> Self &
            requires std::floating_point<T>;

        template <typename Self>
        [[nodiscard]]
        constexpr auto normalized(this const Self &self) noexcept
            -> std::remove_cvref_t<Self>
            requires std::floating_point<T>;

        template <typename Self>
        [[nodiscard]]
        constexpr auto dot(this const Self &self,
                           const std::remove_cvref_t<Self> &other) noexcept
            -> T;

        template <typename Self>
        [[nodiscard]]
        constexpr auto project(this const Self &self,
                               const std::remove_cvref_t<Self> &other) noexcept
            -> std::remove_cvref_t<Self>
            requires std::floating_point<T>;

        ///////////////////////////////////////////////////////////////////////
        // Vector & Vector operations (Member Operators)
        ///////////////////////////////////////////////////////////////////////

        template <typename Self>
        constexpr auto &operator+=(this Self &self,
            const std::remove_cvref_t<Self> &other) noexcept;

        template <typename Self>
        constexpr auto &operator-=(this Self &self,
            const std::remove_cvref_t<Self> &other) noexcept;

        template <typename Self>
        constexpr auto &operator*=(this Self &self,
            const std::remove_cvref_t<Self> &other) noexcept;

        template <typename Self>
        constexpr auto &operator/=(this Self &self,
            const std::remove_cvref_t<Self> &other) noexcept;

        ///////////////////////////////////////////////////////////////////////
        // Vector & Scalar operations (Member Operators)
        ///////////////////////////////////////////////////////////////////////

        template <typename Self>
        constexpr auto &operator+=(this Self &self, T scalar) noexcept;

        template <typename Self>
        constexpr auto &operator-=(this Self &self, T scalar) noexcept;

        template <typename Self>
        constexpr auto &operator*=(this Self &self, T scalar) noexcept;

        template <typename Self>
        constexpr auto &operator/=(this Self &self, T scalar) noexcept;

        ///////////////////////////////////////////////////////////////////////
        // Vector & Vector operations (Friend Operators)
        ///////////////////////////////////////////////////////////////////////

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator+(ConcreteVec lhs,
                                        const ConcreteVec &rhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            lhs += rhs;
            return lhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator-(ConcreteVec lhs,
                                        const ConcreteVec &rhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            lhs -= rhs;
            return lhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator*(ConcreteVec lhs,
                                        const ConcreteVec &rhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            lhs *= rhs;
            return lhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator/(ConcreteVec lhs,
                                        const ConcreteVec &rhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            lhs /= rhs;
            return lhs;
        }

        ///////////////////////////////////////////////////////////////////////
        // Vector & Scalar operations (Friend Operators)
        ///////////////////////////////////////////////////////////////////////

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator+(ConcreteVec lhs, T scalar) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            lhs += scalar;
            return lhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator-(ConcreteVec lhs, T scalar) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            lhs -= scalar;
            return lhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator*(ConcreteVec lhs, T scalar) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            lhs *= scalar;
            return lhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator/(ConcreteVec lhs, T scalar) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            lhs /= scalar;
            return lhs;
        }

        ///////////////////////////////////////////////////////////////////////
        // Scalar & Vector operations (Friend Operators)
        ///////////////////////////////////////////////////////////////////////

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator+(T scalar, ConcreteVec rhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            rhs += scalar;
            return rhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator*(T scalar, ConcreteVec rhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            rhs *= scalar;
            return rhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator-(T scalar, ConcreteVec rhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            for (std::size_t i = 0; i < N; ++i)
                rhs[i] = scalar - rhs[i];
            return rhs;
        }

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator/(T scalar,
                                        const ConcreteVec &rhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            ConcreteVec result;
            for (std::size_t i = 0; i < N; ++i)
                result[i] = scalar / rhs[i];
            return result;
        }

        ///////////////////////////////////////////////////////////////////////
        // Negation
        ///////////////////////////////////////////////////////////////////////

        template <typename ConcreteVec>
        [[nodiscard]]
        friend constexpr auto operator-(ConcreteVec lhs) noexcept
            -> ConcreteVec
            requires std::derived_from<ConcreteVec, VecBase>
        {
            for (std::size_t i = 0; i < N; ++i)
                lhs[i] = -lhs[i];
            return lhs;
        }

        ///////////////////////////////////////////////////////////////////////
        // IO
        ///////////////////////////////////////////////////////////////////////

        template <typename ConcreteVec>
        friend std::ostream &operator<<(std::ostream &os,
                                        const ConcreteVec &v)
            requires std::derived_from<ConcreteVec, VecBase>
        {
            os << "(";
            for (std::size_t i = 0; i < N - 1; ++i)
                os << v[i] << ", ";
            os << v[N - 1];
            os << ")";
            return os;
        }
    };
}

    #include "VecBase.tpp"

#endif /* !RTYPE_VECBASE_HPP_ */
