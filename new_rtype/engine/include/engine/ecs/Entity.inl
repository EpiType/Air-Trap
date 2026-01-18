/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Entity.inl
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
 * @file Entity.inl
 * @brief Entity inline implementations
 * @author Robin Toillon
 * @details Implements constexpr constructors and accessor methods for
 * the Entity class, including index/generation extraction and null
 * entity checks.
 */

namespace aer::ecs
{
    constexpr Entity::Entity(std::uint32_t index, std::uint32_t generation)
        : _id{static_cast<std::uint64_t>(generation) << GEN_SHIFT |
              static_cast<std::uint64_t>(index)}
    {
    }

    constexpr Entity::Entity(void)
        : _id{0}
    {
    }

    constexpr std::uint32_t Entity::index(void) const
    {
        return static_cast<std::uint32_t>(this->_id & INDEX_MASK);
    }

    constexpr std::uint32_t Entity::generation(void) const
    {
        return static_cast<std::uint32_t>(this->_id >> GEN_SHIFT);
    }

    constexpr Entity::operator std::uint64_t(void) const noexcept
    {
        return this->_id;
    }

    constexpr bool Entity::isNull(void) const noexcept
    {
        return this->_id == 0;
    }
}
