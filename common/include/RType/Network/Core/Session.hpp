/**
 * File   : Session.hpp
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
    #include <asio/read.hpp>
    #include <asio/write.hpp>

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type protocol
 */
namespace rtp::net
{
    class IEventPublisher; /**< Forward declaration of IEventPublisher */

    /**
     * @enum PlayerState
     * @brief Enum representing the state of a player in the session
     */
    enum class PlayerState {
        None,
        Lobby,
        InRoom,
        InGame
    };

    /**
     * @class Session
     * @brief Represents a network session with a unique identifier.
     * It create a a shared pointer to itself for safe management.
     */
    class Session : public std::enable_shared_from_this<Session> {
        public:
            /**
            * @brief Constructor for Session
            * @param socket UDP socket for the session
            * @param manager Reference to the NetworkManager managing this session
            */
            Session(asio::ip::tcp::socket _socket, IEventPublisher &publisher);
            
            /**
            * @brief Destructor for Session
            */
            ~Session();

            ////////////////////////////////////////////////
            // Network Core Operations
            ////////////////////////////////////////////////

            /**
             * @brief Start the session's read and write coroutines
             */
            void start(void);

            /**
             * @brief Stop the session
             */
            void stop(void);

            /**
             * @brief Check if the session is connected
             * @return true if connected, false otherwise
             */
            bool isConnected(void) const;

            /**
             * @brief Send a packet over TCP
             * @param packet Packet to be sent
             */
            void sendTcp(const Packet &packet);

            /**
             * @brief Send a packet over UDP
             * @param packet Packet to be sent
             */
            void setUdp(const asio::ip::udp::endpoint& endpoint);

            /**
             * @brief Get the UDP endpoint of the session
             * @return UDP endpoint
             */
            asio::ip::udp::endpoint getUdpEndpoint() const;

            /**
             * @brief Check if the session has a valid UDP endpoint
             * @return true if UDP endpoint is set, false otherwise
             */
            bool hasUdp() const;

            //////////////////////////////////////////////////
            // Session State/Data Accessors
            //////////////////////////////////////////////////

            /**
             * @brief Set the unique identifier of the session
             * @param id New session ID
             */
            void setId(uint32_t id);

            /**
             * @brief Get the unique identifier of the session
             * @return Current session ID
             */
            uint32_t getId() const;

            /**
             * @brief Reset the session timer
             * @note Used for managing timeouts
             */
            void resetTimer(void);

        private:
            /**
             * @brief Coroutine for reading data from the TCP socket
             * @return Awaitable representing the read operation
             */
            asio::awaitable<void> reader(void);

            /**
             * @brief Coroutine for writing data to the TCP socket
             * @return Awaitable representing the write operation
             */
            asio::awaitable<void> writer(void);

        private:
            /* === Network Core === */
            uint32_t _id = 0;                        /**< Unique session identifier */
            bool _stopped = false;                   /**< Indicates if the session is stopped */
            IEventPublisher& _publisher;             /**< Reference to the event publisher */
            asio::ip::tcp::socket _socket;           /**< TCP socket for the session */
            asio::ip::udp::endpoint _udpEndpoint;    /**< UDP endpoint for the session */

            /* === Network data === */
            std::mutex _writeMutex;                  /**< Mutex for thread-safe write queue access */
            asio::steady_timer _timer;               /**< Timer for managing timeouts */
            std::deque<Packet> _writeQueue;          /**< Queue of packets to be sent */
    };
} // namespace rtp::net

#endif /* !RTYPE_NETWORK_SESSION_HPP_ */
