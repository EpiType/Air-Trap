/**
 * File   : IEventPublisher.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_EVENT_SESSION_HPP_
    #define RTYPE_EVENT_SESSION_HPP_

    #include "RType/Network/Packet.hpp"
    #include "RType/Network/INetwork.hpp"

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type protocol
 */
namespace rtp::net
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
            virtual void publishEvent(NetworkEvent event) = 0;
    };
} // namespace rtp::net

#endif /* !RTYPE_NETWORK_SESSION_HPP_ */