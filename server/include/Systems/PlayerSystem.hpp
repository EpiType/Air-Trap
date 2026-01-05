/**
 * File   : PlayerSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_PLAYER_SYSTEM_HPP_
    #define RTYPE_PLAYER_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"
    #include "Game/Player.hpp"

    #include <map>

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {
    /**
     * @class PlayerSystem
     * @brief System to handle player-related operations on the server side.
     */
    class PlayerSystem : public rtp::ecs::ISystem {
        public:
            /** 
             * @brief Constructor for PlayerSystem
             * @param network Reference to the server network manager
             * @param registry Reference to the entity registry
             */
            PlayerSystem(ServerNetwork& network, rtp::ecs::Registry& registry);

            /**
             * @brief Update system logic for one frame
             * @param dt Time elapsed since last update in seconds
             * @note Currently not used
             */
            void update(float dt) override;

            /**
             * @brief Create a new player for the given session ID
             * @param sessionId ID of the network session
             * @param username Username of the player
             */
            PlayerPtr createPlayer(uint32_t sessionId, const std::string& username);

            /**
             * @brief Remove a player by their session ID
             * @param sessionId ID of the network session
             * @return The entity ID associated with the removed player, or 0 if not found
             */
            uint32_t removePlayer(uint32_t sessionId);

            /**
             * @brief Update the username of a player
             * @param sessionId ID of the network session
             * @param username New username of the player
             */
            void updatePlayerUsername(uint32_t sessionId, const std::string& username);

            /**
             * @brief Get a player by their session ID
             * @param sessionId ID of the network session
             * @return Shared pointer to the Player, or nullptr if not found
             */
            PlayerPtr getPlayer(uint32_t sessionId) const;

        private:
            ServerNetwork& _network;                  /**< Reference to the server network manager */
            rtp::ecs::Registry& _registry;            /**< Reference to the entity registry */

        private:
            std::map<uint32_t, PlayerPtr> _players;   /**< Map of session ID to Player instances */
    };
} // namespace rtp::server

#endif /* !RTYPE_PLAYER_SYSTEM_HPP_ */