/**
 * File   : PlayerSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_SERVER_PLAYER_SYSTEM_HPP_
    #define RTYPE_SERVER_PLAYER_SYSTEM_HPP_

    #include "rtype/game/Player.hpp"

    #include <cstdint>
    #include <memory>
    #include <string>
    #include <unordered_map>

namespace rtp::server::systems
{
    /**
     * @class PlayerSystem
     * @brief System for managing players.
     */
    class PlayerSystem {
        public:
            /**
             * @brief Shared pointer type for Player
             */
            using PlayerPtr = std::shared_ptr<Player>;

            /**
             * @brief Create a new player and add it to the system.
             * @param sessionId Session identifier for the player
             * @param username Username of the player
             * @return Shared pointer to the created Player
             */
            PlayerPtr createPlayer(uint32_t sessionId, const std::string &username);

            /**
             * @brief Retrieve a player by session ID.
             * @param sessionId Session identifier of the player
             * @return Shared pointer to the Player, or nullptr if not found
             */
            PlayerPtr getPlayer(uint32_t sessionId) const;

            /**
             * @brief Retrieve a player by username.
             * @param username Username of the player
             * @return Shared pointer to the Player, or nullptr if not found
             */
            PlayerPtr getPlayerByUsername(const std::string &username) const;

            /**
             * @brief Remove a player from the system by session ID.
             * @param sessionId Session identifier of the player to remove
             * @return Session ID of the removed player
             */
            uint32_t removePlayer(uint32_t sessionId);

            /**
             * @brief Update the username of a player.
             * @param sessionId Session identifier of the player
             * @param username New username to set
             */
            void updatePlayerUsername(uint32_t sessionId, const std::string &username);

        private:
            std::unordered_map<uint32_t, PlayerPtr> _players;       /**< Map of session ID to Player instances */
    };
}

#endif /* !RTYPE_SERVER_PLAYER_SYSTEM_HPP_ */
