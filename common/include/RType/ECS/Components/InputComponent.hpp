/**
 * File   : InputComponent.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ECS_COMPONENTS_SERVER_INPUTCOMPONENT_HPP_
    #define RTYPE_ECS_COMPONENTS_SERVER_INPUTCOMPONENT_HPP_

    #include <cstdint>

/**
 * @namespace rtp::ecs::components::server
 * @brief ECS components for R-Type server
 */
namespace rtp::ecs::components::server
{
    /**
     * @struct InputComponent
     * @brief Component to handle network input for server entities
     * 
     * This component is responsible for receiving and processing network input
     * for entities on the server side. It utilizes the ServerNetwork to poll for
     * incoming network events and manage session data.
     */
    struct InputComponent {
        enum InputBits : uint8_t {
            MoveUp    = 1 << 0,
            MoveDown  = 1 << 1,
            MoveLeft  = 1 << 2,
            MoveRight = 1 << 3
        };
        uint8_t mask = 0;                          /**< Input mask for filtering input types */
        uint32_t lastProcessedTick = 0;            /**< Last processed server tick for input */
    };
} // namespace rtp::ecs::components::server

#endif /* !RTYPE_ECS_COMPONENTS_SERVER_INPUTCOMPONENT_HPP_ */