/**
 * File   : INetwork.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_NETWORK_INETWORK_HPP_
    #define RTYPE_NETWORK_INETWORK_HPP_

    #include "RType/Network/Packet.hpp"

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type protocol
 */
namespace rtp::net {

    /**
     * @enum NetworkMode
     * @brief Enum representing network transmission modes
     * @note TCP will be used for reliable data, UDP for real-time data
     */
    enum class NetworkMode {
        TCP,
        UDP
    };

    /**
     * @struct NetworkEvent
     * @brief Represents a network event containing session ID and packet data
     */
    struct NetworkEvent {
        uint32_t sessionId;
        Packet packet;
    };

    /**
     * @class INetwork
     * @brief Interface for network operations
     * 
     * This interface defines the essential network operations such as starting/stopping the network,
     * sending packets, broadcasting packets, and polling for network events.
     */
    class INetwork {
        public:
            /**
             * @brief Virtual destructor
             */
            virtual ~INetwork() = default;

            /**
             * @brief Start the network on the specified port
             * @param port Port number to start the network on
             */
            virtual void start(uint16_t port) = 0;

            /**
             * @brief Stop the network
             */
            virtual void stop(void) = 0;

            /**
             * @brief Send a packet to a specific session
             * @param sessionId ID of the session to send the packet to - Only for the server
             * @param packet Packet to be sent
             * @param mode Network mode (TCP/UDP)
             */
            virtual void sendPacket(uint32_t sessionId, const Packet &packet, NetworkMode mode) = 0;
            
            /**
             * @brief Broadcast a packet to all sessions
             * @param packet Packet to be broadcasted
             * @param mode Network mode (TCP/UDP)
             */
            virtual void broadcastPacket(const Packet &packet, NetworkMode mode) = 0;

            /**
             * @brief Poll for a network event
             * @param event Reference to store the polled network event
             * @return True if an event was polled, false otherwise
             */
            virtual std::optional<NetworkEvent> pollEvent(void) = 0;
    };
}

#endif  /* !RTYPE_NETWORK_INETWORK_HPP_ */