/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Entity.hpp
*/

#ifndef RTYPE_ENTITY_HPP_
    #define RTYPE_ENTITY_HPP_

    #include <compare>
    #include <cstdint>
    #include <limits>

namespace rtp::ecs
{
    /**
     * @typedef Entity
     * @brief Represents an entity in the ECS
     * (Entity-Component-System) architecture.
     *
     * @details An Entity
     * is represented as a 32-bit unsigned integer.
     * It serves as a unique
     * identifier for entities within the ECS framework.
     * Entities are
     * used to associate components and systems in the ECS design pattern.
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

    constexpr Entity NullEntity{};
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

#endif /* !RTYPE_ENTITY_HPP_ */
