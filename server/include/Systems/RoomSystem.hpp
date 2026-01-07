/**
 * File   : RoomSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ROOM_SYSTEM_HPP_
    #define RTYPE_ROOM_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"
    
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/Network/Packet.hpp"
    #include "Systems/ServerNetworkSystem.hpp"
    #include "Game/Room.hpp"
    
    #include <memory>
    #include <map>

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {
    /**
    * @class RoomSystem
    * @brief System to handle room-related operations on the server side.
    */
    class RoomSystem : public rtp::ecs::ISystem
    {
        public:
            //////////////////////////////////////////////////////////////////////////
            // Recived data structures
            //////////////////////////////////////////////////////////////////////////

            struct JoinRoomData {
                uint32_t roomId;
            };

            struct CreateRoomData {
                char roomName[32];
                uint8_t maxPlayers;
                float difficulty;
                float speed;
            };

        public:
            /** 
             * @brief Constructor for RoomSystem
             * @param network Reference to the server network manager
             * @param registry Reference to the entity registry
             */
            RoomSystem(ServerNetwork& network, rtp::ecs::Registry& registry);

            /**
             * @brief Update system logic for one frame
             * @param dt Time elapsed since last update in seconds
             * @note Currently not used
             */
            void update(float dt) override;

            /**
             * @brief Create a new room based on client request
             * @param sessionId ID of the network session
             * @param packet Packet containing the room creation data
             */
            uint32_t createRoom(uint32_t sessionId, const std::string &roomName, uint8_t maxPlayers,
                            float difficulty, float speed, Room::RoomType type);

            /**
             * @brief Join an existing room based on client request
             * @param sessionId ID of the network session
             * @param packet Packet containing the room join data
             */
            bool joinRoom(PlayerPtr player, uint32_t roomId);

            /**
             * @brief Join the lobby room
             * @param sessionId ID of the network session
             */
            bool joinLobby(PlayerPtr player);

            /**
             * @brief Leave the current room based on client request
             * @param sessionId ID of the network session
             * @param packet Packet containing the room leave data
             */
            bool leaveRoom(PlayerPtr player);

            /**
             * @brief Disconnect a player from all rooms
             * @param sessionId ID of the network session
             */
            void disconnectPlayer(uint32_t sessionId);

            /**
             * @brief List all available rooms based on client request
             * @param sessionId ID of the network session
             * @param packet Packet containing the room list request data
             */
            void listAllRooms(uint32_t sessionId);

            /**
             * @brief Handle chat message in the room based on client request
             * @param sessionId ID of the network session
             * @param packet Packet containing the chat message data
             */
            void chatInRoom(uint32_t sessionId, const rtp::net::Packet& packet);

            /**
             * @brief Launch all rooms that are ready to start the game
             */
            void launchReadyRooms(float dt);

        private:
            ServerNetwork& _network;                          /**< Reference to the server network manager */
            rtp::ecs::Registry& _registry;                    /**< Reference to the entity registry */
            std::map<uint32_t, std::shared_ptr<Room>> _rooms; /**< Map of room ID to Room instances */
            std::map<uint32_t, uint32_t> _playerRoomMap;      /**< Map of player session ID to room ID */
            uint32_t _nextRoomId = 1;                         /**< Next available room ID */
            uint32_t _lobbyId = 0;                            /**< ID of the main lobby room */  
            mutable std::mutex _mutex;                        /**< Mutex for thread-safe operations */

    };
} // namespace rtp::server

#endif /* !RTYPE_ROOM_SYSTEM_HPP_ */