/**
 * File   : IEventPublisher.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef ENGINE_EVENT_SESSION_HPP_
    #define ENGINE_EVENT_SESSION_HPP_

    #include "engine/net/NetworkUtils.hpp"

/**
 * @namespace aer::net
 * @brief Network layer for network
 */
namespace aer::net
{
    /**
     * @class IEventPublisher
     * @brief Interface for network event handling
     * 
     * This interface defines the method for publishing network events.
     */
    class IEventPublisher {
        public:
            /**
             * @brief Virtual default destructor
             */
            virtual ~IEventPublisher() = default;

            /**
             * @brief Get buffer sequence for network transmission
             * @return Buffer sequence containing header and body
             */
            virtual void publishEvent(aer::net::NetworkEvent event) = 0;
    };
} // namespace aer::net

#endif /* !ENGINE_EVENT_SESSION_HPP_ */