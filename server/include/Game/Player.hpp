/**
 * File   : Player.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

 #ifndef RTYPE_GAME_PLAYER_HPP_
    #define RTYPE_GAME_PLAYER_HPP_

    #include <cstdint>
    #include <string>
    #include <mutex>
    #include <memory>
    #include "RType/Network/Core/Session.hpp"

/**
 * @namespace rtp::server
 * @brief Game logic for R-Type server
 */
namespace rtp::server
{
    /**
     * @enum PlayerState
     * @brief Enum representing the state of a player in the session
     */
    enum class PlayerState {
        None,
        Connected,
        InLobby,
        InGame
    };

    /**
     * @class Player
     * @brief Represents a player in the game server.
     */
    class Player {
        public:
            /**
             * @brief Constructor for Player
             * @param id Unique identifier for the player
             * @param username Username of the player
             */
            Player(uint32_t sessionId, const std::string &username);

            /**
             * @brief Destructor for Player
             */
            ~Player();

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
             * @brief Get the unique identifier of the Session
             * @return Current Session ID
             */
            uint32_t getId() const;

        private:
            uint32_t _sessionId;         /**< Unique player identifier */
            std::string _username;       /**< Player username */
            uint32_t _roomId;            /**< ID of the room the player is in ; by default 0 (no room) */
            PlayerState _state;          /**< Current state of the player */
            bool _isReady;               /**< Readiness status of the player */

            mutable std::mutex _mutex;   /**< Mutex to protect access to player state */
    };

    /**
     * @typedef PlayerPtr
     * @brief Shared pointer type for Player
     */
    using PlayerPtr = std::shared_ptr<Player>;

} // namespace rtp::server
#endif /* !RTYPE_GAME_PLAYER_HPP_ */