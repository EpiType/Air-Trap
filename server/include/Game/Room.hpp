/**
 * File   : Room.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_GAME_ROOM_HPP_
    #define RTYPE_GAME_ROOM_HPP_

    #include <cstdint>
    #include <string>
    #include <vector>
    #include <mutex>    
    #include "Game/Player.hpp"
    #include <memory>
    #include <list>

/**
 * @namespace rtp::server
 * @brief Game logic for R-Type server
 */
namespace rtp::server
{
    /**
     * @class Room
     * @brief Represents a game room in the server.
     * A Room can hold multiple players and manage their states.
     */
    class Room {
        public:
            /**
             * @enum State
             * @brief Enum representing the state of the room
             */
            enum State {
                Waiting,        /**< Waiting for players */  
                InGame,         /**< Game in progress */
                Finished        /**< Game finished */
            };

            /**
             * @enum Type
             * @brief Enum representing the type of the room
             */
            enum RoomType {
                Lobby,          /**< Lobby room type, Only used for the principal Lobby */
                Public,         /**< Public room type, can be joined by anyone */
                Private         /**< Private room type, hosted by the player */
            };

            /**
             * @enum PlayerType
             * @brief Enum representing the type of player in the room
             */
            enum PlayerType {
                Player,        /**< Regular player */
                Spectator      /**< Spectator player */
            };

            /**
             * @enum PlayerMode
             * @brief Enum representing the mode of a player in the room
             */
            enum PlayerMode {
                Member,       /**< Regular member */
                Administrator /**< Room administrator */
            };

            /**
             * @brief Constructor for Room
             * @param id Unique identifier for the room
             * @param name Name of the room
             * @param maxPlayers Maximum number of players allowed in the room
             */
            Room(uint32_t id, const std::string &name, uint32_t maxPlayers, RoomType type);

            /**
             * @brief Destructor for Room
             */
            ~Room();

            /**
             * @brief Add a player to the room
             * @param player Shared pointer to the player to add
             * @return true if the player was added successfully, false if the room is full
             */
            bool canJoin() const;

            /**
             * @brief Add a player to the room
             * @param player Shared pointer to the player to add
             * @return true if the player was added successfully, false if the room is full
             */
            bool addPlayer(const PlayerPtr &player);

            /**
             * @brief Remove a player from the room
             * @param sessionId Unique identifier of the player to remove
             */
            void removePlayer(uint32_t sessionId);

            /**
             * @brief Get the list of players in the room
             * @return Vector of shared pointers to players in the room
             */
            const std::list<PlayerPtr> getPlayers(void) const;

            /**
             * @brief Get the unique identifier of the room
             * @return Current room ID
             */
            uint32_t getId(void) const;

            /**
             * @brief Get the type of the room
             * @return Current room type
             */
            RoomType getType() const;

            /**
             * @brief Get the name of the room
             * @return Current room name
             */
            std::string getName(void) const;

            /**
             * @brief Get the maximum number of players allowed in the room
             * @return Maximum player count
             */
            uint32_t getMaxPlayers(void) const;

            /**
             * @brief Get the current number of players in the room
             * @return Current player count
             */
            uint32_t getCurrentPlayerCount(void) const;

            /**
             * @brief Get the current state of the room
             * @return Current room state
             */
            State getState() const;

        private:
            uint32_t _id;                     /**< Unique room identifier */
            std::string _name;                /**< Name of the room */
            uint32_t _maxPlayers;             /**< Maximum number of players allowed */
            std::list<PlayerPtr> _players;    /**< List of player ptr's in the room */
            State _state;                     /**< Current state of the room */
            RoomType _type;                       /**< Type of the room */

            mutable std::mutex _mutex;        /**< Mutex to protect access to room state */
    };
} // namespace rtp::server

#endif /* !RTYPE_GAME_ROOM_HPP_ */