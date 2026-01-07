/**
 * File   : GameManager.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_GAME_GAMEMANAGER_HPP_
    #define RTYPE_GAME_GAMEMANAGER_HPP_

    #include <cstdint>
    #include <string>
    #include <vector>
    #include <memory>
    #include <map>
    #include <mutex>
    #include <thread>
    #include <chrono>
    
    #include "RType/Logger.hpp"
    #include "Game/Room.hpp"
    #include "Game/Player.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"
    #include "RType/ECS/Registry.hpp"

    /* Systems */
    #include "Systems/ServerNetworkSystem.hpp"
    #include "Systems/MovementSystem.hpp"
    #include "Systems/AuthSystem.hpp"
    #include "Systems/RoomSystem.hpp"
    #include "Systems/PlayerSystem.hpp"
    #include "Systems/EntitySystem.hpp"

    /* Components */
    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/EntityType.hpp"

/**
 * @namespace rtp::server
 * @brief R-Type server-side game management
 */
namespace rtp::server
{
    /**
     * @class GameManager
     * @brief Manages game state, rooms, and player interactions on the server side.
     */
    class GameManager {
        public:
            /**
             * @brief Constructor for GameManager
             * @param networkManager Reference to the ServerNetwork instance
             */
            GameManager(ServerNetwork &networkManager);

            /**
             * @brief Destructor for GameManager
             */
            ~GameManager();

            /**
             * @brief Main game loop
             * @note Processes network events, updates game state, and manages rooms
             */
            void gameLoop(void);

            /**
             * @brief Start a game in the specified room
             * @param room Reference to the Room where the game will start
             */
            void startGame(Room &room);

        private:
            ////////////////////////////////////////////////////////////////////////////
            // Network Event Processing
            ////////////////////////////////////////////////////////////////////////////
            
            /**
             * @brief Process incoming network events with OpCode handling
             * @note Handles player connections, disconnections, and packet processing
             */
            void processNetworkEvents(void);

            /**
             * @brief Handle a new player connection
             * @param sessionId Unique identifier of the connecting player
             */
            void handlePlayerConnect(uint32_t sessionId);

            /**
             * @brief Handle a player disconnection
             * @param sessionId Unique identifier of the disconnecting player
             */
            void handlePlayerDisconnect(uint32_t sessionId);

            /**
             * @brief Handle player login with provided username if successful then join the lobby
             * @param sessionId Unique identifier of the player
             * @param username Player's username
             */
            void handlePlayerLoginAuth(uint32_t sessionId, const net::Packet &packet);

            /**
             * @brief Handle player registration with provided username
             * @param sessionId Unique identifier of the player
             * @param username Player's username
             */
            void handlePlayerRegisterAuth(uint32_t sessionId, const net::Packet &packet);

            /**
             * @brief Handle a request to list available rooms
             * @param sessionId Unique identifier of the player requesting the room list
             * @param packet Reference to the received Packet
             */
            void handleListRooms(uint32_t sessionId);

            /**
             * @brief Handle a generic incoming packet from a player
             * @param sessionId Unique identifier of the player sending the packet
             * @param packet Reference to the received Packet
             */
            void handleCreateRoom(uint32_t sessionId, const net::Packet &packet);

            /**
             * @brief Handle a generic incoming packet from a player
             * @param sessionId Unique identifier of the player sending the packet
             * @param packet Reference to the received Packet
             */
            void handleJoinRoom(uint32_t sessionId, const net::Packet &packet);

            /**
             * @brief Handle a generic incoming packet from a player
             * @param sessionId Unique identifier of the player sending the packet
             * @param packet Reference to the received Packet
             */
            void handleLeaveRoom(uint32_t sessionId, const net::Packet &packet);

            /**
             * @brief Handle a generic incoming packet from a player
             * @param sessionId Unique identifier of the player sending the packet
             * @param packet Reference to the received Packet
             */
            void handleSetReady(uint32_t sessionId, const net::Packet &packet);

            /**
             * @brief Handle a generic incoming packet from a player
             * @param sessionId Unique identifier of the player sending the packet
             * @param packet Reference to the received Packet
             */
            void handleRoomChatSended(uint32_t sessionId, const net::Packet &packet);

            /**
             * @brief Handle an incoming packet from a player
             * @param sessionId Unique identifier of the player sending the packet
             * @param packet Reference to the received Packet
             */
            void handlePacket(uint32_t sessionId, const net::Packet &packet);

        private:
            ServerNetwork &_networkManager;                            /**< Reference to the ServerNetwork instance */

            rtp::ecs::Registry _registry;                              /**< ECS Registry for managing entities and components */

            std::unique_ptr<ServerNetworkSystem> _serverNetworkSystem; /**< Server network system for handling network-related ECS operations */
            std::unique_ptr<MovementSystem> _movementSystem;           /**< Movement system for updating entity positions */
            std::unique_ptr<AuthSystem> _authSystem;                   /**< Authentication system for handling player logins */
            std::unique_ptr<RoomSystem> _roomSystem;                   /**< Room system for handling room management */
            std::unique_ptr<PlayerSystem> _playerSystem;               /**< Player system for handling player-related operations */
            std::unique_ptr<EntitySystem> _entitySystem;               /**< Entity system for handling entity-related operations */

            uint32_t _serverTick = 0;                                  /**< Current server tick for synchronization */
            mutable std::mutex _mutex;                                 /**< Mutex for thread-safe operations */
    };
}

#endif // RTYPE_GAME_GAMEMANAGER_HPP_