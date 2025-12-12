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

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type protocol
 */
namespace rtp::net
{

    class NetworkManager;

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
            Session(asio::ip::tcp::socket _socket, NetworkManager &manager);
            
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
            bool hasUdp() const;

            //////////////////////////////////////////////////
            // Session State/Data Accessors
            //////////////////////////////////////////////////

            /**
             * @brief Set the username of the player
             * @param username New username
             */
            void setUsername(const std::string &username);

            /**
             * @brief Get the username of the player
             * @return Current username
             */
            std::string getUsername() const;

            /**
            * @brief Set the room ID the player is in
            * @param roomId New room ID
            */
            void setRoomId(uint32_t roomId);

            /**
             * @brief Get the room ID the player is in
             * @return Current room ID
             */
            uint32_t getRoomId() const;

            /**
             * @brief Check if the player is in a room
             * @return true if in a room, false otherwise
             */
            bool isInRoom() const;

            /**
             * @brief Set the state of the player
             * @param state New player state
             */
            void setState(PlayerState state);

            /**
             * @brief Get the state of the player
             * @return Current player state
             */
            PlayerState getState() const;

            /**
             * @brief Set the readiness status of the player
             * @param ready New readiness status
             */
            void setReady(bool ready);

            /**
             * @brief Get the readiness status of the player
             * @return Current readiness status
             */
            bool isReady() const;

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
            NetworkManager& _manager;                /**< Reference to the network manager */
            asio::ip::tcp::socket _socket;           /**< TCP socket for the session */
            asio::ip::udp::endpoint _udpEndpoint;    /**< UDP endpoint for the session */

            /* === Network data === */
            std::mutex _writeMutex;                  /**< Mutex for thread-safe write queue access */
            asio::steady_timer _timer;               /**< Timer for managing timeouts */
            std::deque<Packet> _writeQueue;          /**< Queue of packets to be sent */

            /* === Session State/Data === */
            uint32_t _roomId = 0;                    /**< ID of the room the player is in ; by default 0 (no room) */
            bool _isReady = false;                   /**< Player readiness status */
            mutable std::mutex _stateMutex;          /**< Mutex for thread-safe state access */
            std::string _username = "Unknown";       /**< Player username */
            PlayerState _state = PlayerState::None;  /**< Current state of the player */

    };
} // namespace rtp::net

#endif /* !RTYPE_NETWORK_SESSION_HPP_ */
