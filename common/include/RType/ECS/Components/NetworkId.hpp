/**
 * File   : NetworkId.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ECS_COMPONENTS_NETWORKID_HPP_
    #define RTYPE_ECS_COMPONENTS_NETWORKID_HPP_

    #include <cstdint>

/**
 * @namespace rtp::ecs::components::server
 * @brief ECS components for R-Type server
 */
namespace rtp::ecs::components {
    /**
     * @struct NetworkId
     * @brief Component representing a network identifier for an entity.
     */
    struct NetworkId {
        uint32_t id;     /**< Unique network identifier for the entity */
    };
} // namespace rtp::ecs::components

#endif /* !RTYPE_ECS_COMPONENTS_NETWORKID_HPP_ */