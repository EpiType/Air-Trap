/**
 * File   : ServerNetworkManager.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_NETWORK_ServerNetworkManager_HPP_
    #define RTYPE_NETWORK_ServerNetworkManager_HPP_

    #include "RType/Network/INetwork.hpp"
    #include "RType/Network/Core/Session.hpp"
    #include "RType/Network/Packet.hpp"
    #include "RType/Network/IEventPublisher.hpp"
    #include "RType/Logger.hpp"

    #include <asio.hpp>
    #include <asio/write.hpp>
    #include <memory>
    #include <unordered_map>
    #include <thread>
    #include <mutex>
    #include <queue>

    using namespace rtp::net;
    using namespace rtp::log;

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type protocol
 */
namespace rtp::server {
    /**
     * @class ServerNetworkManager
     * @brief ASIO-based implementation of the INetwork interface.
     * 
     * This class provides network functionalities using ASIO for TCP and UDP
     * communication. It manages sessions, handles incoming connections and
     * packets, and provides methods for sending and broadcasting packets.
     */
    class ServerNetwork : public INetwork, public IEventPublisher {
        public:
            /**
             * @brief Constructor for ServerNetworkManager
             * @param port Port number to start the server on
             * @note Initializes ASIO components and prepares for accepting connections
             */
            ServerNetwork(uint16_t port);

            /**
             * @brief Destructor for ServerNetworkManager
             * @note Cleans up ASIO resources and stops the I/O context
             */
            ~ServerNetwork() override;

            /**
             * @brief Start the network server
             * @note Begins accepting TCP connections and receiving UDP packets
             * and starts the I/O context thread
             */
            void start(void) override;

            /**
             * @brief Stop the network server
             * @note Closes all sessions and stops the I/O context
             */
            void stop(void) override;

            /**
             * @brief Send a packet to a specific session
             * @param sessionId ID of the session to send the packet to
             * @param packet Packet to be sent
             * @param mode Network mode (TCP/UDP)
             */
            void sendPacket(uint32_t sessionId, const Packet &packet, NetworkMode mode);

            /**
             * @brief Broadcast a packet to all sessions
             * @param packet Packet to be broadcasted
             * @param mode Network mode (TCP/UDP)
             */
            void broadcastPacket(const Packet &packet, NetworkMode mode);

            /**
             * @brief Poll for a network event
             * @return Optional containing the polled network event if available
             */
            std::optional<NetworkEvent> pollEvent(void) override;

            /**
             * @brief Publish a network event
             * @param event Network event to be published
             */
            void publishEvent(NetworkEvent event) override;

            /**
             * @brief Get the next available session ID
             * @return Next session ID
             */
            uint32_t getNextSessionId(void);

        private:
            /**
             * @brief Handler for accepted TCP connections
             * @param error Error code from the accept operation
             * @param socket Socket for the accepted connection
             */
            asio::awaitable<void> runTcpAcceptor();

            /**
             * @brief Handler for received UDP packets
             * @param error Error code from the receive operation
             * @param bytesTransferred Number of bytes received
             */
            asio::awaitable<void> runUdpReader();

            /**
             * @brief Helper function to send a UDP packet to a specific endpoint
             * @param endpoint Destination UDP endpoint
             * @param packet Packet to be sent
             */
            void sendUdpHelper(const asio::ip::udp::endpoint &endpoint, const Packet &packet);

        private:
            std::unordered_map<uint32_t,
            std::shared_ptr<rtp::net::Session>> _sessions; /**< Active sessions */
            
            std::unordered_map<asio::ip::udp::endpoint,
            uint32_t> _udpEndpointToId;                    /**< Mapping of UDP endpoints to session IDs */
            
            std::mutex _sessionsMutex;                     /**< Mutex for thread-safe session access */
            
            std::queue<NetworkEvent> _eventQueue;          /**< Queue of network events */
            std::mutex _eventQueueMutex;                   /**< Mutex for thread-safe event queue access */
            
            asio::io_context _ioContext;                   /**< ASIO I/O context */
            asio::ip::tcp::acceptor _tcpAcceptor;          /**< TCP acceptor for incoming connections */
            asio::ip::udp::socket _udpSocket;              /**< UDP socket for receiving packets */
            std::thread _ioThread;                         /**< Thread for running the I/O context */

            uint32_t _nextSessionId;                       /**< Next available session ID */

            uint16_t _port;                                /**< Port number the server is running on */
    };
} // namespace rtp::net

#endif /* !RTYPE_NETWORK_ServerServerNetworkManager_HPP_ */