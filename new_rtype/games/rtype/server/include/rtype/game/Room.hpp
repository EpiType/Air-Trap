/**
 * File   : Room.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_SERVER_ROOM_HPP_
    #define RTYPE_SERVER_ROOM_HPP_

    #include "rtype/net/Packet.hpp"

    #include <cstdint>
    #include <string>
    #include <unordered_set>
    #include <vector>

namespace rtp::server
{
    class Room {
        public:
            /** 
             * @enum RoomType
             * @brief Types of rooms available.
             */
            enum class RoomType {
                Lobby,
                Public
            };

            /** 
             * @enum State
             * @brief States a room can be in.
             */
            enum class State {
                Waiting,
                InGame
            };

            /** 
             * @brief Constructor for Room
             * @param id Unique identifier for the room
             * @param name Name of the room
             * @param maxPlayers Maximum number of players allowed
             * @param difficulty Difficulty level of the room
             * @param speed Speed setting of the room
             * @param duration Duration of the game session
             * @param seed Seed value for randomization
             * @param levelId Level identifier for the room
             */
            Room(uint32_t id,
                 std::string name,
                 uint32_t maxPlayers,
                 float difficulty,
                 float speed,
                 uint32_t duration,
                 uint32_t seed,
                 uint32_t levelId);

            /** 
             * @brief Get the room's unique identifier.
             * @return Room ID
             */
            [[nodiscard]]
            uint32_t id(void) const;

            /** 
             * @brief Get the room's name.
             * @return Room name
             */
            [[nodiscard]]
            const std::string &name(void) const;

            /** 
             * @brief Set the room's name.
             * @param name New room name
             */
            [[nodiscard]]
            uint32_t maxPlayers(void) const;

            /** 
             * @brief Get the room's difficulty level.
             * @return Difficulty level
             */
            [[nodiscard]]
            bool inGame(void) const;

            /** 
             * @brief Set whether the game is in progress.
             * @param inGame True if the game is in progress, false otherwise
             */
            void setInGame(bool inGame);

            /** 
             * @brief Get the room's difficulty level.
             * @return Difficulty level
             */
            [[nodiscard]]
            State state(void) const;

            /** 
             * @brief Set the room's state.
             * @param state New state of the room
             */
            void setState(State state);

            /** 
             * @brief Get the room's type.
             * @return Room type
             */
            [[nodiscard]]
            RoomType type(void) const;

            /** 
             * @brief Set the room's type.
             * @param type New room type
             */
            void setType(RoomType type);

            /** 
             * @brief Set the room's owner.
             * @param sessionId Session ID of the new owner
             */
            void setOwner(uint32_t sessionId);

            /** 
             * @brief Get the room's owner.
             * @return Session ID of the owner
             */
            [[nodiscard]]
            uint32_t owner(void) const;

            /** 
             * @brief Check if a player can join the room.
             * @return True if a player can join, false otherwise
             */
            [[nodiscard]]
            bool canJoin(void) const;

            /** 
             * @brief Add a player to the room.
             * @param sessionId Session ID of the player to add
             * @return True if the player was added, false otherwise
             */
            [[nodiscard]]
            bool addPlayer(uint32_t sessionId);

            /** 
             * @brief Remove a player from the room.
             * @param sessionId Session ID of the player to remove
             * @return True if the player was removed, false otherwise
             */
            [[nodiscard]]
            bool removePlayer(uint32_t sessionId);

            /** 
             * @brief Check if a player is in the room.
             * @param sessionId Session ID of the player to check
             * @return True if the player is in the room, false otherwise
             */
            [[nodiscard]]
            bool hasPlayer(uint32_t sessionId) const;

            /** 
             * @brief Get the current number of players in the room.
             * @return Number of players
             */
            [[nodiscard]]
            std::size_t playerCount(void) const;

            /** 
             * @brief Get the list of player session IDs in the room.
             * @return Vector of player session IDs
             */
            [[nodiscard]]
            const std::vector<uint32_t> &players(void) const;

            /** 
             * @brief Check if a username is banned from the room.
             * @param username Username to check
             * @return True if the username is banned, false otherwise
             */
            [[nodiscard]]
            bool isBanned(const std::string &username) const;

            /** 
             * @brief Ban a username from the room.
             * @param username Username to ban
             */
            void ban(const std::string &username);

            /** 
             * @brief Unban a username from the room.
             * @param username Username to unban
             */
            void unban(const std::string &username);

            /** 
             * @brief Convert the room to a RoomInfo structure.
             * @return RoomInfo representation of the room
             */
            [[nodiscard]]
            rtp::net::RoomInfo toInfo(void) const;

            /** 
             * @brief Get the room's difficulty level.
             * @return Difficulty level
             */
            [[nodiscard]]
            float speed(void) const;

        private:
            uint32_t _id{0};                    /**< Unique identifier for the room */
            std::string _name;                  /**< Name of the room */
            uint32_t _maxPlayers{0};            /**< Maximum number of players allowed */
            float _difficulty{1.0f};            /**< Difficulty level of the room */
            float _speed{1.0f};                 /**< Speed setting of the room */
            uint32_t _duration{0};              /**< Duration of the game session */  
            uint32_t _seed{0};                  /**< Seed value for randomization */
            uint32_t _levelId{0};               /**< Level identifier for the room */
            uint32_t _owner{0};                 /**< Session ID of the room owner */
            RoomType _type{RoomType::Public};   /**< Type of the room */
            State _state{State::Waiting};       /**< Current state of the room */
            std::unordered_set<std::string>
                _banned;                        /**< Set of banned usernames */
            std::vector<uint32_t> _players;     /**< List of player session IDs in the room */
    };
}

#endif /* !RTYPE_SERVER_ROOM_HPP_ */
