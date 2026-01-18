/**
 * File   : GameManager.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_SERVER_GAMEMANAGER_HPP_
    #define RTYPE_SERVER_GAMEMANAGER_HPP_

    #include "engine/net/INetwork.hpp"
    #include "engine/net/NetworkUtils.hpp"
    #include "rtype/systems/AuthSystem.hpp"
    #include "rtype/systems/PlayerSystem.hpp"
    #include "rtype/systems/RoomSystem.hpp"
    #include "rtype/net/Packet.hpp"

    #include <cstdint>
    #include <unordered_map>

namespace rtp::server
{
    /**
     * @class GameManager
     * @brief Main game manager handling network events and game logic.
     */
    class GameManager {
        public:
            /**
             * @brief Constructor for GameManager
             * @param network Reference to the network instance
             */
            explicit GameManager(aer::net::INetwork &network);

            /**
             * @brief Handle a network event.
             * @param event The network event to handle
             */
            void handleEvent(const aer::net::NetworkEvent &event);

            /**
             * @brief Update the game manager.
             * @param dt Delta time since last update
             */
            void update(float dt);

        private:
            /**
             * @brief Handle an incoming packet.
             * @param sessionId Session identifier of the sender
             * @param channel Network channel the packet was received on
             * @param packet The received packet
             */
            void handlePacket(uint32_t sessionId,
                              aer::net::NetChannel channel,
                              rtp::net::Packet packet);

            void handlePlayerConnect(uint32_t sessionId);
            void handlePlayerDisconnect(uint32_t sessionId);
            void handleLogin(uint32_t sessionId, rtp::net::Packet &packet);
            void handleRegister(uint32_t sessionId, rtp::net::Packet &packet);
            void handleListRooms(uint32_t sessionId);
            void handleCreateRoom(uint32_t sessionId, rtp::net::Packet &packet);
            void handleJoinRoom(uint32_t sessionId, rtp::net::Packet &packet);
            void handleLeaveRoom(uint32_t sessionId);
            void handleSetReady(uint32_t sessionId, rtp::net::Packet &packet);
            void handleRoomChat(uint32_t sessionId, rtp::net::Packet &packet);
            void handleInputTick(uint32_t sessionId, rtp::net::Packet &packet);
            void handlePing(uint32_t sessionId, rtp::net::Packet &packet);

            void spawnRoomEntities(uint32_t roomId);
            void sendRoomEntitiesToSession(uint32_t roomId, uint32_t sessionId);
            void broadcastEntityDeath(uint32_t roomId, uint32_t sessionId);

            void broadcastRoomState(float dt);

        private:
            aer::net::INetwork &_network;        /**< Network instance for communication */
            systems::AuthSystem _authSystem;        /**< Authentication system */
            systems::PlayerSystem _playerSystem;    /**< Player management system */
            systems::RoomSystem _roomSystem;        /**< Room management system */

            uint32_t _nextEntityId{1};              /**< Next available entity ID */
            uint32_t _serverTick{0};                /**< Current server tick counter */
    };
}

#endif /* !RTYPE_SERVER_GAMEMANAGER_HPP_ */
