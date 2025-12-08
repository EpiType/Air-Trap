/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Signature.hpp
*/

#ifndef RTYPE_SIGNATURE_HPP_
    #define RTYPE_SIGNATURE_HPP_

    #include <bitset>
    #include <cstdint>

namespace rtp::ecs
{
    constexpr size_t MAX_COMPONENTS = 64; /**< Maximum number of components supported */

    /**
     * @typedef Signature
     * @brief Represents a signature for an entity's components in the ECS architecture.
     *
     * @details A Signature is implemented as a bitset where each bit corresponds to a component type.
     * If a bit is set to 1, it indicates that the entity possesses that component type; if set to 0,
     * the entity does not have that component type. This allows for efficient tracking and querying
     * of component compositions for entities within the ECS framework.
     */
    using Signature = std::bitset<MAX_COMPONENTS>;
}

#endif /* !RTYPE_SIGNATURE_HPP_ */
