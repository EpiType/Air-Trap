/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** NumericConcept class implementation
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
 * @file NumericConcept.hpp
 * @brief Definition of the Numeric concept.
 * @author Robin Toillon
 */

#ifndef RTYPE_NUMERIC_CONCEPT_HPP_
    #define RTYPE_NUMERIC_CONCEPT_HPP_

    #include <concepts>
    #include <type_traits>

namespace rtp
{
    /**
     * @brief A concept that checks
     *        if a type is numeric (integral or floating-point).
     *
     * @tparam T The type to check.
     */
    template<typename T>
    concept Numeric = std::is_arithmetic_v<T>;

}

#endif /* !RTYPE_NUMERIC_CONCEPT_HPP_ */
