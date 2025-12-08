/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Entity.inl
*/

namespace rtp::ecs
{
    constexpr Entity::Entity(std::uint32_t index, std::uint32_t generation)
        : _id{static_cast<std::uint64_t>(generation) << GEN_SHIFT |
              static_cast<std::uint64_t>(index) & INDEX_MASK}
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
