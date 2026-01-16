/**
 * File   : NetworkId.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef ENGINE_ECS_COMPONENTS_NETWORKID_HPP_
    #define ENGINE_ECS_COMPONENTS_NETWORKID_HPP_

    #include <cstdint>

/**
 * @namespace engine::ecs::components::server
 * @brief ECS components for R-Type server
 */
namespace engine::ecs::components {
    /**
     * @struct NetworkId
     * @brief Component representing a network identifier for an entity.
     */
    struct NetworkId {
        uint32_t id;     /**< Unique network identifier for the entity */
    };
} // namespace engine::ecs::components

#endif /* !ENGINE_ECS_COMPONENTS_NETWORKID_HPP_ */