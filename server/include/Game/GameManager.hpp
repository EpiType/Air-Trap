/**
 * File   : GameManager.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */


#ifndef RTYPE_GAME_GameManager_HPP_
    #define RTYPE_GAME_GameManager_HPP_

    #include <cstdint>
    #include <string>
    #include <vector>
    #include <memory>
    #include <map>
    #include "Game/Room.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"

    using namespace rtp::net;

/**
 * @namespace rtp::server
 * @brief Game logic for R-Type server
 */
namespace rtp::server
{
    /**
     * @class GameManager
     * @brief Manages game rooms on the server.
     * Responsible for creating, deleting, and retrieving rooms.
     */
    class GameManager {
        public:
            GameManager(ServerNetwork &networkManager);

            /**
             * @brief Destructor for GameManager
             */
            ~GameManager();

            /**
             * @brief Main game loop to handle room updates
             */
            void gameLoop(void);

        private:

            /**
             * @brief Process incoming network events
             * @note Handles player connections, disconnections, and packets
             */
            void processNetworkEvents(void);

            /**
             * @brief Handle a new player connection
             * @param sessionId Unique identifier of the connected player
             */
            void handlePlayerConnect(uint32_t sessionId);

            /**
             * @brief Handle a player disconnection
             * @param sessionId Unique identifier of the disconnected player
             */
            void handlePlayerDisconnect(uint32_t sessionId);

            /**
             * @brief Handle an incoming packet from a player
             * @param sessionId Unique identifier of the player
             * @param packet Packet received from the player
             */
            void handlePacket(uint32_t sessionId, const Packet &packet);

            /**
             * @brief Attempt to join a player to a lobby
             * @param player Shared pointer to the player attempting to join
             */
            void tryJoinLobby(PlayerPtr player, uint32_t roomId = 0);

            /**
             * @brief Send updated room information to all players in the room
             * @param room Reference to the room to send updates for
             */
            void sendLobbyUpdate(const Room &room);

            ServerNetwork &_networkManager;                                  /**< Reference to the network manager */
            std::map<uint32_t, std::shared_ptr<Room>> _rooms;           /**< Map of room ID to Room instances */
            std::map<uint32_t, uint32_t> _playerRoomMap;                /**< Map of player session ID to room ID */
            uint32_t _nextRoomId = 1;                                   /**< Next available room ID */
    
            mutable std::mutex _mutex;                                  /**< Mutex to protect access to rooms */

    };
} // namespace rtp::server

#endif // RTYPE_GAME_GameManager_HPP_