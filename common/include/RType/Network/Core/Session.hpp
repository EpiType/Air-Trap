/**
 * File   : NetworkId.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_NETWORK_SESSION_HPP_
    #define RTYPE_NETWORK_SESSION_HPP_

    #include <asio.hpp>
    #include <cstdint>
    #include <memory>
    #include <deque>
    #include <mutex>
    #include "RType/Network/Packet.hpp"
    #include "RType/Network/INetwork.hpp"
    #include "RType/Network/IEventPublisher.hpp"

namespace rtp::net
{
    class Session : public std::enable_shared_from_this<Session> {
        public:
            /**
             * @brief Constructor for Session
             * @param id Unique identifier for the session
             * @param socket TCP socket associated with the session
             * @param serverUdpSocket Reference to the server's UDP socket
             * @param publisher Reference to the event publisher for network events
             */
            Session(uint32_t id, 
                    asio::ip::tcp::socket socket, 
                    asio::ip::udp::socket& serverUdpSocket,
                    IEventPublisher &publisher);
            
            /**
             * @brief Destructor for Session
             */
            ~Session();

            /**
             * @brief Start the session's read and write operations
             * @note Initiates asynchronous reading from the TCP socket
             */
            void start(void);

            /**
             * @brief Stop the session's operations
             * @note Closes the TCP socket and stops any ongoing operations
             */
            void stop(void);

            /**
             * @brief Send a packet to the client
             * @param packet Packet to be sent
             * @param mode Network mode (TCP or UDP) for sending the packet
             */
            void send(const Packet& packet, NetworkMode mode);

            /**
             * @brief Set the unique identifier for the session
             * @param id New identifier for the session
             */
            void setId(uint32_t id);

            /**
             * @brief Get the unique identifier for the session
             * @return Current identifier of the session
             */
            uint32_t getId() const;

            /**
             * @brief Set the TCP endpoint for the session
             * @param endpoint New TCP endpoint for the session
             */
            asio::ip::tcp::endpoint getTcpEndpoint() const;

            /**
             * @brief Set the UDP endpoint for the session
             * @param endpoint New UDP endpoint for the session
             */
            void setUdpEndpoint(const asio::ip::udp::endpoint& endpoint);

            /**
             * @brief Get the UDP endpoint for the session
             * @return Current UDP endpoint of the session
             */
            asio::ip::udp::endpoint getUdpEndpoint() const { return _udpEndpoint; }

        private:
            /**
             * @brief Asynchronous reader coroutine for the session
             * @note Continuously reads packets from the TCP socket
             */
            asio::awaitable<void> reader(void);

            /**
             * @brief Asynchronous writer coroutine for the session
             * @note Continuously writes packets to the TCP socket
             */
            asio::awaitable<void> writer(void);

        private:
            uint32_t _id;                             /**< Unique identifier for the session */
            bool _stopped = false;                    /**< Flag indicating if the session is stopped */
            
            asio::ip::tcp::socket _socket;            /**< TCP socket associated with the session */
            asio::ip::udp::socket& _serverUdpSocket;  /**< Reference to the server's UDP socket */
            IEventPublisher& _publisher;              /**< Reference to the event publisher for network events */
            
            asio::ip::udp::endpoint _udpEndpoint{};   /**< UDP endpoint associated with the session */
            bool _hasUdp = false;                     /**< Flag indicating if UDP endpoint is set */

            std::mutex _writeMutex;                   /**< Mutex for synchronizing write operations */
            asio::steady_timer _timer;                /**< Timer for managing write operations */
            std::deque<Packet> _writeQueue{};         /**< Queue of packets to be written to the TCP socket */
    };
}

#endif /* !RTYPE_NETWORK_SESSION_HPP_ */