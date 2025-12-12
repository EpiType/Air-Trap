/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Entity.hpp
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
 * @file Entity.hpp
 * @brief Entity identifier for ECS architecture
 * @author Robin Toillon
 * @details Provides a unique entity identifier using index and
 * generation numbers to support entity recycling while maintaining
 * validity checks.
 */

#ifndef RTYPE_ENTITY_HPP_
    #define RTYPE_ENTITY_HPP_

    #include <compare>
    #include <cstdint>
    #include <limits>
    #include <functional>

namespace rtp::ecs
{
    /**
     * @class Entity
     * @brief Represents an entity in the ECS (Entity-Component-System)
     * architecture
     * @details An Entity is represented as a 64-bit identifier combining
     * a 32-bit index and a 32-bit generation counter. It serves as a
     * unique identifier for entities within the ECS framework. Entities
     * are used to associate components and systems in the ECS design
     * pattern. The generation counter allows entity IDs to be recycled
     * safely.
     */
    class Entity {
        public:
            static constexpr std::uint32_t MAX_INDEX =
                            std::numeric_limits<std::uint32_t>::max();
            static constexpr std::uint64_t INDEX_MASK = MAX_INDEX;
            static constexpr std::uint64_t GEN_SHIFT = 32;

            constexpr Entity(std::uint32_t index, std::uint32_t generation);

            constexpr Entity(void);

            [[nodiscard]]
            constexpr std::uint32_t index(void) const;

            [[nodiscard]]
            constexpr std::uint32_t generation(void) const;

            constexpr auto operator<=>(const Entity &) const = default;
    
            constexpr operator std::uint64_t(void) const noexcept;

            [[nodiscard]]
            constexpr bool isNull(void) const noexcept;

        private:
            std::uint64_t _id; /**< The unique identifier for the entity */

            friend struct std::hash<Entity>;
    };
}

namespace std
{
    template <>
    struct hash<rtp::ecs::Entity> {
        size_t operator()(const rtp::ecs::Entity &e) const
        {
            return hash<std::uint64_t>{}(e._id);
        }
    };
}

    #include "Entity.inl"

namespace rtp::ecs
{
    constexpr Entity NullEntity{};
}

#endif /* !RTYPE_ENTITY_HPP_ */
