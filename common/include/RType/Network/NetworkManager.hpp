/**
 * File   : NetworkManager.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_NETWORK_NetworkManager_HPP_
    #define RTYPE_NETWORK_NetworkManager_HPP_

    #include "RType/Network/INetwork.hpp"
    #include "RType/Network/Core/Session.hpp"

    #include <asio.hpp>
    #include <asio/write.hpp>
    #include <memory>
    #include <unordered_map>
    #include <thread>
    #include <mutex>
    #include <queue>

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type protocol
 */
namespace rtp::net {

    /**
     * @class NetworkManager
     * @brief ASIO-based implementation of the INetwork interface.
     * 
     * This class provides network functionalities using ASIO for TCP and UDP
     * communication. It manages sessions, handles incoming connections and
     * packets, and provides methods for sending and broadcasting packets.
     */
    class NetworkManager : public INetwork {
        public:
            NetworkManager();
            ~NetworkManager() override;

            void start(uint16_t port) override;
            void stop(void) override;

            void sendPacket(uint32_t sessionId, const Packet &packet, NetworkMode mode) override;
            void broadcastPacket(const Packet &packet, NetworkMode mode) override;

            std::optional<NetworkEvent> pollEvent(void) override;

            /**
             * @brief Publish a network event to the event queue
             * @param event Network event to be published
             */
            void publishEvent(NetworkEvent event);

        private:
            /**
             * @brief Handler for accepted TCP connections
             * @param error Error code from the accept operation
             * @param socket Socket for the accepted connection
             */
            asio::awaitable<void> runTcpAcceptor(const asio::error_code &error, asio::ip::tcp::socket socket);

            /**
             * @brief Handler for received UDP packets
             * @param error Error code from the receive operation
             * @param bytesTransferred Number of bytes received
             */
            asio::awaitable<void> runUdpReader(const asio::error_code &error, size_t bytesTransferred);

            /**
             * @brief Helper function to send a UDP packet to a specific endpoint
             * @param endpoint Destination UDP endpoint
             * @param packet Packet to be sent
             */
            void sendUdpHelper(const asio::ip::udp::endpoint &endpoint, const Packet &packet);

        private:
            asio::io_context _ioContext;             /**< ASIO I/O context */
            asio::ip::tcp::acceptor _tcpAcceptor;    /**< TCP acceptor for incoming connections */
            asio::ip::udp::socket _udpSocket;        /**< UDP socket for receiving packets */
            std::thread _ioThread;                   /**< Thread for running the I/O context */

            std::unordered_map<uint32_t,
                std::shared_ptr<rtp::net::Session>> _sessions; /**< Active sessions */
            
            std::mutex _sessionsMutex;               /**< Mutex for thread-safe session access */

            std::queue<NetworkEvent> _eventQueue;    /**< Queue of network events */
            std::mutex _eventQueueMutex;             /**< Mutex for thread-safe event queue access */

            uint32_t _nextSessionId;                 /**< Next available session ID */
    };
} // namespace rtp::net

#endif /* !RTYPE_NETWORK_NetworkManager_HPP_ */