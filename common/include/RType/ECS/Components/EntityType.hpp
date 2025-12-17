/**
 * File   : EntityType.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ECS_COMPONENTS_ENTITYTYPE_HPP_
#define RTYPE_ECS_COMPONENTS_ENTITYTYPE_HPP_

    #include "RType/Math/Vec2.hpp"
    #include "RType/Network/Packet.hpp"

/**
 * @namespace rtp::ecs::components
 * @brief ECS components for R-Type protocol
 */
namespace rtp::ecs::components {
    /**
     * @struct EntityTypeComp
     * @brief Component representing the type of an entity.
     */
    struct EntityType {
        rtp::net::EntityType type = rtp::net::EntityType::Player;
    };
} // namespace rtp::ecs::components

#endif /* !RTYPE_ECS_COMPONENTS_ENTITYTYPE_HPP_ */