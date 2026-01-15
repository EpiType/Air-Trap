/**
 * File   : INetwork.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_SERVER_NETWORK_HPP_
    #define RTYPE_SERVER_NETWORK_HPP_

    #include "RType/Network/INetwork.hpp"
    #include "RType/Network/Session.hpp"
    #include "RType/Network/Packet.hpp"
    #include "RType/Network/IEventPublisher.hpp"
    #include "RType/Logger.hpp"

    #include <asio.hpp>
    #include <memory>
    #include <unordered_map>
    #include <thread>
    #include <mutex>
    #include <queue>
    #include <optional>

/**
 * @namespace rtp::server
 * @brief Network layer for R-Type server protocol
 */
namespace rtp::server {

    /**
     * @class ServerNetwork
     * @brief Implementation ASIO du serveur réseau (TCP + UDP)
     */
    class ServerNetwork : public net::INetwork, public net::IEventPublisher {
        public:
            /**
             * @brief Constructor for ServerNetwork
             * @param port Port number to start the server on
             */
            ServerNetwork(uint16_t port);

            /**
             * @brief Destructor for ServerNetwork
             */
            ~ServerNetwork() override;

            /**
             * @brief Start the network server
             * @note Begins accepting connections and starts the I/O context thread
             */
            void start(void) override;

            /**
             * @brief Stop the network server
             * @note Closes connections and stops the I/O context
             */
            void stop(void) override;

            /**
             * @brief Send a packet to a specific session
             * @param sessionId ID of the session to send the packet to
             * @param packet Packet to send
             * @param mode Network mode (TCP or UDP)
             */
            void sendPacket(uint32_t sessionId, const net::Packet &packet, net::NetworkMode mode);

            /**
             * @brief Broadcast a packet to all connected sessions
             * @param packet Packet to broadcast
             * @param mode Network mode (TCP or UDP)
             */
            void broadcastPacket(const net::Packet &packet, net::NetworkMode mode);

            /**
             * @brief Poll for a network event
             * @return Optional NetworkEvent if available
             */
            std::optional<net::NetworkEvent> pollEvent(void) override;

            /**
             * @brief Publish a network event to be processed
             * @param event NetworkEvent to publish
             */
            void publishEvent(net::NetworkEvent event) override;

        private:
            /**
             * @brief Accept incoming TCP connections
             * @note Initiates an asynchronous accept operation
             */
            void acceptConnection();

            /**
             * @brief Receive incoming UDP packets
             * @note Initiates an asynchronous receive operation
             */
            void receiveUdpPacket();

        private:
            asio::io_context _ioContext;                       /**< ASIO I/O context for managing asynchronous operations */
            asio::ip::tcp::acceptor _acceptor;                 /**< TCP acceptor for incoming connections */
            asio::ip::udp::socket _udpSocket;                  /**< UDP socket for receiving and sending datagrams */
            std::thread _ioThread;                             /**< Thread running the ASIO I/O context */

            std::unordered_map<uint32_t, 
                std::shared_ptr<net::Session>> _sessions; /**< Map of active sessions indexed by session ID */
            std::mutex _sessionsMutex;                         /**< Mutex for protecting access to the sessions map */
            
            std::queue<net::NetworkEvent> _eventQueue;    /**< Queue of network events to be processed */
            std::mutex _eventQueueMutex;                       /**< Mutex for protecting access to the event queue */
            
            uint32_t _nextSessionId;                           /**< Next session ID to assign to a new connection */

            std::unordered_map<asio::ip::udp::endpoint,
                uint32_t> _udpEndpointToSessionId;             /**< Map of UDP endpoints to session IDs */
            std::mutex _udpMapMutex;                           /**< Mutex for protecting access to the UDP endpoint map */

            std::array<char, 4096> _udpBuffer;                 /**< Buffer for receiving UDP packets */
            asio::ip::udp::endpoint _udpRemoteEndpoint;        /**< Remote endpoint for the last received UDP packet */
    };
} 

#endif /* !RTYPE_SERVER_NETWORK_HPP_ */