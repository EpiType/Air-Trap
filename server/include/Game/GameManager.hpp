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
    #include "Systems/NetworkSyncSystem.hpp"
    #include "Systems/MovementSystem.hpp"
    #include "Systems/AuthSystem.hpp"
    #include "Systems/RoomSystem.hpp"
    #include "Systems/PlayerSystem.hpp"
    #include "Systems/EntitySystem.hpp"
    #include "Systems/PlayerMouvementSystem.hpp"
    #include "Systems/PlayerShootSystem.hpp"
    #include "Systems/EnemyAISystem.hpp"
    #include "Systems/LevelSystem.hpp"
    #include "Systems/CollisionSystem.hpp"
    #include "Systems/EnemyShootSystem.hpp"
    #include "Systems/BulletCleanupSystem.hpp"

    /* Components */
    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/ECS/Components/SimpleWeapon.hpp"
    #include "RType/ECS/Components/Ammo.hpp"
    #include "RType/ECS/Components/MouvementPattern.hpp"
    #include "RType/ECS/Components/Health.hpp"
    #include "RType/ECS/Components/BoundingBox.hpp"
    #include "RType/ECS/Components/Damage.hpp"
    #include "RType/ECS/Components/Powerup.hpp"
    #include "RType/ECS/Components/MovementSpeed.hpp"
    #include "RType/ECS/Components/Shield.hpp"
    #include "RType/ECS/Components/DoubleFire.hpp"

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
             * @brief Handle an update to the selected weapon sent by client
             */
            void handleUpdateSelectedWeapon(uint32_t sessionId, const net::Packet &packet);

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
            void handlePing(uint32_t sessionId, const net::Packet &packet);

            bool handleChatCommand(PlayerPtr player, const std::string& message);
            void sendChatToSession(uint32_t sessionId, const std::string& message);
            void sendSystemMessageToRoom(uint32_t roomId, const std::string& message);

            void sendEntitySpawnToSessions(const ecs::Entity& entity,
                                           const std::vector<uint32_t>& sessions);
            void sendRoomEntitySpawnsToSession(uint32_t roomId, uint32_t sessionId);

        private:
            ServerNetwork &_networkManager;                            /**< Reference to the ServerNetwork instance */

            ecs::Registry _registry;                              /**< ECS Registry for managing entities and components */

            std::unique_ptr<NetworkSyncSystem> _networkSyncSystem; /**< Server network system for handling network-related ECS operations */
            std::unique_ptr<MovementSystem> _movementSystem;           /**< Movement system for updating entity positions */
            std::unique_ptr<AuthSystem> _authSystem;                   /**< Authentication system for handling player logins */
            std::unique_ptr<RoomSystem> _roomSystem;                   /**< Room system for handling room management */
            std::unique_ptr<PlayerSystem> _playerSystem;               /**< Player system for handling player-related operations */
            std::unique_ptr<EntitySystem> _entitySystem;               /**< Entity system for handling entity-related operations */
            std::unique_ptr<PlayerMouvementSystem> _playerMouvementSystem; /**< Player movement system for handling player-specific movement logic */
            std::unique_ptr<PlayerShootSystem> _playerShootSystem;      /**< Player shooting system for handling bullets */
            std::unique_ptr<EnemyAISystem> _enemyAISystem;              /**< Enemy AI system for movement patterns */
            std::unique_ptr<LevelSystem> _levelSystem;                  /**< Level system for timed spawns */
            std::unique_ptr<CollisionSystem> _collisionSystem;          /**< Collision system for pickups/obstacles */
            std::unique_ptr<EnemyShootSystem> _enemyShootSystem;        /**< Enemy shooting system */
            std::unique_ptr<BulletCleanupSystem> _bulletCleanupSystem;  /**< Bullet cleanup system */

            uint32_t _serverTick = 0;                                  /**< Current server tick for synchronization */
            mutable std::mutex _mutex;                                 /**< Mutex for thread-safe operations */
            bool _gamePaused = false;                                  /**< Global game pause flag */
            float _gameSpeed = 1.0f;                                   /**< Global game speed multiplier */
    };
}

#endif // RTYPE_GAME_GAMEMANAGER_HPP_
