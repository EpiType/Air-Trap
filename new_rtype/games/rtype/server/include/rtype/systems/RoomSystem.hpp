/**
 * File   : RoomSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_SERVER_ROOM_SYSTEM_HPP_
    #define RTYPE_SERVER_ROOM_SYSTEM_HPP_

    #include "engine/net/INetwork.hpp"
    #include "rtype/game/Room.hpp"
    #include "rtype/systems/PlayerSystem.hpp"

    #include <cstdint>
    #include <functional>
    #include <mutex>
    #include <unordered_map>
    #include <vector>

namespace rtp::server::systems
{
    /**
     * @class RoomSystem
     * @brief System for managing game rooms.
     */
    class RoomSystem {
        public:
            using RoomPtr = std::shared_ptr<Room>;                  /**< Shared pointer type for Room */
            using PlayerPtr = PlayerSystem::PlayerPtr;              /**< Shared pointer type for Player */
            using OnRoomStarted = std::function<void(uint32_t)>;    /**< Callback type for room started event */

            /**
             * @brief Constructor for RoomSystem
             * @param network Reference to the network instance
             * @param players Reference to the PlayerSystem instance
             */
            RoomSystem(aer::net::INetwork &network, PlayerSystem &players);

            /**
             * @brief Create a new room.
             * @param sessionId Session identifier of the room owner
             * @param roomName Name of the room
             * @param maxPlayers Maximum number of players allowed
             * @param difficulty Difficulty level of the room
             * @param speed Speed setting of the room
             * @param type Type of the room (Lobby or Public)
             * @param levelId Level identifier for the room
             * @param seed Seed value for randomization
             * @param duration Duration of the game in the room
             * @return Identifier of the created room
             */
            uint32_t createRoom(uint32_t sessionId,
                                const std::string &roomName,
                                uint8_t maxPlayers,
                                float difficulty,
                                float speed,
                                Room::RoomType type,
                                uint32_t levelId,
                                uint32_t seed,
                                uint32_t duration);

            /**
             * @brief Player joins a room.
             * @param player Shared pointer to the player
             * @param roomId Identifier of the room to join
             * @param asSpectator True if joining as a spectator, false otherwise
             * @return True if the player successfully joined the room, false otherwise
             */
            bool joinRoom(const PlayerPtr &player, uint32_t roomId, bool asSpectator);

            /**
             * @brief Player leaves their current room.
             * @param player Shared pointer to the player
             * @return True if the player successfully left the room, false otherwise
             */
            bool joinLobby(const PlayerPtr &player);

            /**
             * @brief Player leaves their current room.
             * @param player Shared pointer to the player
             * @return True if the player successfully left the room, false otherwise
             */
            bool leaveRoom(const PlayerPtr &player);

            /**
             * @brief Disconnect a player from the system.
             * @param sessionId Session identifier of the player to disconnect
             */
            void disconnectPlayer(uint32_t sessionId);

            /**
             * @brief List all available rooms to a player.
             * @param sessionId Session identifier of the player
             */
            void listAllRooms(uint32_t sessionId);

            /**
             * @brief Launch all ready rooms.
             * @param dt Delta time since last update
             */
            void launchReadyRooms(float dt);

            /**
             * @brief Retrieve a room by its identifier.
             * @param roomId Identifier of the room
             * @return Shared pointer to the Room, or nullptr if not found
             */
            RoomPtr getRoom(uint32_t roomId);

            /**
             * @brief Retrieve all rooms.
             * @return Vector of shared pointers to all Room instances
             */
            std::vector<RoomPtr> rooms(void) const;

            /**
             * @brief Get the lobby room identifier.
             * @return Identifier of the lobby room
             */
            uint32_t lobbyId(void) const;

            /**
             * @brief Set the callback to be invoked when a room starts.
             * @param cb Callback function taking the room ID as parameter
             */
            void setOnRoomStarted(OnRoomStarted cb);

        private:
            aer::net::INetwork &_network;        /**< Network instance for communication */
            PlayerSystem &_players;                 /**< Reference to the PlayerSystem instance */
            std::unordered_map<uint32_t,
                RoomPtr> _rooms;                    /**< Map of room ID to Room instances */
            std::unordered_map<uint32_t,
                uint32_t> _playerRoomMap;           /**< Map of session ID to current room ID */
            uint32_t _nextRoomId{1};                /**< Next available room ID */
            uint32_t _lobbyId{0};                   /**< Identifier of the lobby room */
            OnRoomStarted _onRoomStarted{};         /**< Callback for room started event */
            mutable std::mutex _mutex;              /**< Mutex for thread-safe operations */
    };
}

#endif /* !RTYPE_SERVER_ROOM_SYSTEM_HPP_ */
