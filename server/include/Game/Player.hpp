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
    #include "RType/Network/Session.hpp"
    #include "RType/ECS/Components/SimpleWeapon.hpp"

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
        NotLogged,
        Connected,
        InLobby,
        InRoom,
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
            Player(uint32_t sessionId);

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
             * @brief Set the entity ID associated with the player
             * @param entityId New entity ID
             */
            void setEntityId(uint32_t entityId);

            /**
             * @brief Get the unique identifier of the Session
             * @return Current Session ID
             */
            uint32_t getId() const;

            /**
             * @brief Get the associated entity ID
             * @return Current entity ID
             */
            uint32_t getEntityId() const;

            void setMuted(bool muted);
            bool isMuted() const;

            /**
             * @brief Set the selected weapon kind for this player
             * @param weapon Weapon kind
             */
            void setWeaponKind(rtp::ecs::components::WeaponKind weapon);

            /**
             * @brief Get the selected weapon kind for this player
             * @return Current weapon kind
             */
            rtp::ecs::components::WeaponKind getWeaponKind() const;

            /**
             * @brief Add to player's score (can be negative)
             * @param delta Score delta
             */
            void addScore(int delta);

            /**
             * @brief Set player's score
             * @param score New score
             */
            void setScore(int score);

            /**
             * @brief Get player's score
             * @return Current score
             */
            int getScore() const;

        private:
            uint32_t _sessionId;         /**< Unique player identifier */
            std::string _username;       /**< Player username */
            uint32_t _roomId;            /**< ID of the room the player is in ; by default 0 (no room) */
            PlayerState _state;          /**< Current state of the player */
            bool _isReady;               /**< Readiness status of the player */

            uint32_t _entityId = 0;      /**< Associated entity ID in the ECS */
            mutable std::mutex _mutex;   /**< Mutex to protect access to player state */
            bool _isMuted = false;       /**< Mute status of the player */
            rtp::ecs::components::WeaponKind _weaponKind{rtp::ecs::components::WeaponKind::Classic}; /**< Selected weapon */
            int _score{0};               /**< Player score */
    };

    /**
     * @typedef PlayerPtr
     * @brief Shared pointer type for Player
     */
    using PlayerPtr = std::shared_ptr<Player>;

} // namespace rtp::server
#endif /* !RTYPE_GAME_PLAYER_HPP_ */
