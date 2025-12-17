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
    
    #include "Game/Room.hpp"
    #include "Game/Player.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"
    #include "RType/ECS/Registry.hpp"

    /* Systems */
    #include "Systems/ServerNetworkSystem.hpp"
    #include "Systems/MovementSystem.hpp"

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
             * @brief Handle an incoming packet from a player
             * @param sessionId Unique identifier of the player sending the packet
             * @param packet Reference to the received Packet
             */
            void handlePacket(uint32_t sessionId, const net::Packet &packet);
            
            ////////////////////////////////////////////////////////////////////////////
            // Room and Player Management
            ////////////////////////////////////////////////////////////////////////////
            
            /**
             * @brief Attempt to join a player to a lobby or specified room
             * @param player Shared pointer to the Player attempting to join
             * @param roomId Optional room ID to join; defaults to 0 (lobby)
             */
            void tryJoinLobby(PlayerPtr player, uint32_t roomId = 0);

            /**
             * @brief Send a lobby update to all players in the room
             * @param room Reference to the Room to update
             */
            void sendLobbyUpdate(const Room &room);

            /**
             * @brief Spawn an entity for the player in the ECS
             * @param player Shared pointer to the Player for whom to spawn the entity
             */
            void spawnPlayerEntity(PlayerPtr player);

            /**
             * @brief Spawn an enemy entity at the specified position
             * @param position Position where the enemy will be spawned
             */
            void spawnEnemyEntity(const Vec2f &position);

        private:
            ServerNetwork &_networkManager;                            /**< Reference to the ServerNetwork instance */
            std::map<uint32_t, std::shared_ptr<Room>> _rooms;          /**< Map of room ID to Room instances */
            std::map<uint32_t, uint32_t> _playerRoomMap;               /**< Map of player session ID to room ID */
            std::map<uint32_t, PlayerPtr> _players;                    /**< Map of session ID to Player instances */
            uint32_t _nextRoomId = 1;                                  /**< Next available room ID */
    
            rtp::ecs::Registry _registry;                              /**< ECS Registry for managing entities and components */
            std::unique_ptr<ServerNetworkSystem> _serverNetworkSystem; /**< Server network system for handling network-related ECS operations */
            std::unique_ptr<MovementSystem> _movementSystem;           /**< Movement system for updating entity positions */

            uint32_t _serverTick = 0;                                  /**< Current server tick for synchronization */
            mutable std::mutex _mutex;                                 /**< Mutex for thread-safe operations */
    };
}

#endif // RTYPE_GAME_GAMEMANAGER_HPP_