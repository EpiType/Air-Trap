/**
 * File   : Network.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_NETWORK_NETWORK_HPP_
    #define RTYPE_NETWORK_NETWORK_HPP_

    #include "RType/Network/INetwork.hpp"
    #include "RType/Network/Packet.hpp"
    #include "RType/Network/IEventPublisher.hpp"

    #include <memory>
    #include <optional>
    #include <deque>
    #include <mutex>
    #include <asio.hpp>
    #include <thread>
    #include <array>

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type client protocol
 */
namespace rtp::client {

    /**
     * @class Network
     * @brief ASIO-based implementation of the INetwork interface for the client.
     * 
     * This class provides network functionalities using ASIO for TCP and UDP
     * communication. It manages the connection to the server, handles incoming
     * packets, and provides methods for sending packets.
     */

     class ClientNetwork: public net::INetwork, public net::IEventPublisher {
        public:
            /**
             * @brief Constructor for Client Network
             * @param serverIp IP address of the server to connect to
             * @param serverPort Port number of the server to connect to
             * @note Initializes ASIO components and prepares for connection
             */
            ClientNetwork(const std::string &serverIp, uint16_t serverPort);

            /**
             * @brief Destructor for Client Network
             * @note Cleans up ASIO resources and stops the I/O context
             */
            ~ClientNetwork() override;

            /**
             * @brief Start the network client
             * @note Begins connecting to the server and starts the I/O context thread
             */
            void start(void) override;

            /**
             * @brief Stop the network client
             * @note Closes connections and stops the I/O context
             */
            void stop(void) override;

            /**
             * @brief Send a packet to the server
             * @param packet Packet to be sent
             * @param mode Network mode (TCP or UDP)
             */
            void sendPacket(const net::Packet &packet, net::NetworkMode mode);

            /**
             * @brief Poll for a network event
             * @return Optional NetworkEvent if available
             */
            std::optional<net::NetworkEvent> pollEvent(void) override;
 
            /**
             * @brief Publish a network event
             * @param event Network event to be published
             */
            void publishEvent(net::NetworkEvent event);

            /**
             * @brief Check if there are pending packets in the event queue
             * @return Vector of pending packets
             */
            std::vector<net::Packet> hasPendingPackets(void);

            /**
             * @brief Pop a packet from the event queue
             * @return Packet popped from the queue
             */
            net::Packet popPacket(void);

            /**
             * @brief Read TCP packet header asynchronously
             */
            void readTcpHeader(void);

            /**
             * @brief Read TCP packet body asynchronously
             */
            void readTcpBody(void);

            /**
             * @brief Read UDP packets asynchronously
             */
            void readUdp(void);

            /**
             * @brief Send UDP handshake to the server
             */
            void sendUdpHandshake(void);

            /**
             * @brief Check if UDP socket is ready
             * @return true if UDP socket is bound and ready, false otherwise
             */
            bool isUdpReady(void) const;

        private:
            uint16_t _serverPort;                       /**< Server port number */
            std::string _serverIp;                      /**< Server IP address */

            asio::io_context _ioContext;                /**< ASIO I/O context */
            asio::ip::tcp::socket _tcpSocket;           /**< TCP socket for communication */
            asio::ip::udp::socket _udpSocket;           /**< UDP socket for communication */
            asio::ip::udp::endpoint _serverEndpoint;    /**< Server UDP endpoint */

            std::thread _ioThread;                      /**< Thread for running the I/O context */

            std::mutex _eventQueueMutex;                /**< Mutex for event queue synchronization */
            std::deque<net::NetworkEvent> _eventQueue;  /**< Queue of network events */

            net::Header _tcpHeader;                     /**< TCP packet header */
            net::Header _udpHeader;                     /**< UDP packet header */
            std::vector<uint8_t> _tcpBody;              /**< TCP packet body */

            uint32_t _sessionId = 0;                    /**< Client session ID */
            bool _udpBound = false;                     /**< Flag indicating if UDP is bound */
            
            std::array<char, 65536> _udpBuffer;         /**< Buffer for UDP packets */
            asio::ip::udp::endpoint _udpSenderEndpoint; /**< Endpoint of the UDP sender */

     };
}; // namespace rtp::client

#endif /* !RTYPE_NETWORK_NETWORK_HPP_ */
