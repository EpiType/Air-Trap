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
    #include <algorithm>

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
                None,          /**< Undefined player type */
                Player,        /**< Regular player */
                Spectator      /**< Spectator player */
            };

        public:
            /**
             * @brief Constructor for Room
             * @param id Unique identifier for the room
             * @param name Name of the room
             * @param maxPlayers Maximum number of players allowed in the room
             */
            Room(uint32_t id, const std::string &name, uint32_t maxPlayers, float difficulty, float speed, RoomType type);

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
             * @brief Get the difficulty level of the room
             * @return Current difficulty level
             */
            float getDifficulty() const;

            /**
             * @brief Get the speed multiplier of the room
             * @return Current speed multiplier
             */
            float getSpeed() const;

            /**
             * @brief Get the type of a player in the room
             * @param sessionId Unique identifier of the player
             * @return PlayerType of the specified player
             */
            PlayerType getPlayerType(uint32_t sessionId) const;

            /**
             * @brief Set the type of a player in the room
             * @param sessionId Unique identifier of the player
             * @param type New PlayerType to set
             */
            void setPlayerType(uint32_t sessionId, PlayerType type);

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
            std::list<std::pair<PlayerPtr, PlayerType>>
                _players;                    /**< List of player ptr's in the room */
            State _state;                     /**< Current state of the room */
            RoomType _type;                   /**< Type of the room */
            uint32_t _creatorSessionId;       /**< Session ID of the room creator (Administrator) */

            std::pair<std::string, int>
                _bestRoomScore;               /**< Best score achieved in the room */
                
            uint32_t _levelId;                /**< Level identifier for the room */
            uint32_t _seed;                   /**< Seed for random generation in the room */
            float _difficulty;                /**< Difficulty multiplier for the room 0 -> 1 */
            float _speed;                     /**< Speed multiplier for the room 0 -> 2 */
            uint32_t durationMinutes;         /**< Duration of the game in minutes */
                
            uint32_t _currentTimeSeconds;     /**< Current time in seconds since the game started */
            mutable std::mutex _mutex;        /**< Mutex to protect access to room state */
    };
} // namespace rtp::server

#endif /* !RTYPE_GAME_ROOM_HPP_ */