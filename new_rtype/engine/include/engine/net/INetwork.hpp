/**
 * File   : INetwork.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_ENGINE_NET_INETWORK_HPP_
    #define RTYPE_ENGINE_NET_INETWORK_HPP_

    #include <cstdint>
    #include <string>
    #include <vector>
    #include <optional>
    #include "engine/net/NetworkUtils.hpp"

/**
 * @namespace engine::net
 * @brief Networking components and structures for the engine
 */
namespace engine::net
{
    /**
     * @class INetwork
     * @brief Interface for network implementations.
     * 
     * This interface defines the contract for network plugins that can be
     * loaded into the engine. Implementations must provide methods for
     * initializing the network, sending and receiving packets, and managing
     * connections.
     */
    class INetwork {
        public:
            virtual ~INetwork() = default;

            /** 
             * @brief Start the network service.
             * @return true if the network started successfully, false otherwise
             */
            virtual bool start(void) = 0;

            /** 
             * @brief Stop the network service.
             */
            virtual void stop(void) = 0;

            /** 
             * @brief Send a packet to a specific session.
             * @param sessionId The ID of the session to send the packet to
             * @param payload The data payload to send
             * @param channel The network channel to use for sending
             */
            virtual void sendPacket(uint32_t sessionId,
                                    ByteSpan payload,
                                    NetChannel channel) = 0;

            /** 
             * @brief Poll for incoming network events.
             * @return An optional NetworkEvent if an event is available,
             *         std::nullopt otherwise
             */
            virtual std::optional<NetworkEvent> pollEvent(void) = 0;
    };
}

#endif /* !RTYPE_ENGINE_NET_INETWORK_HPP_ */
