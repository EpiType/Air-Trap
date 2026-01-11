/**
 * File   : NetworkSyncSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_CLIENT_NETWORK_SYSTEM_HPP_
    #define RTYPE_CLIENT_NETWORK_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "Network/ClientNetwork.hpp"
    
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/Sprite.hpp"
    #include "RType/ECS/Components/Animation.hpp"
    #include "RType/Network/Packet.hpp"

    #include "Game/EntityBuilder.hpp"

    #include <vector>
    #include <iostream>
    #include <list>

/**
 * @namespace rtp::client
 * @brief Systems for R-Type client
 */
namespace rtp::client {

    /**
     * @class NetworkSyncSystem
     * @brief System to handle network-related operations on the client side.
     */
    class NetworkSyncSystem : public rtp::ecs::ISystem
    {
        public:
            enum class State {
                NotLogged,
                InLobby,
                JoiningRoom,
                CreatingRoom,
                LeavingRoom,
                InRoom,
                InGame
            };
        public:
            /** 
             * @brief Constructor for NetworkSyncSystem
             * @param network Reference to the client network manager
             * @param registry Reference to the entity registry
             */
            NetworkSyncSystem(ClientNetwork& network, rtp::ecs::Registry& registry, Client::Game::EntityBuilder builder);

            /**
             * @brief Update system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

            /**
             * @brief Send a request to server attempting to log in with provided credentials
             * @param sessionId ID of the network session
             * @param username Username string
             * @param password Password string
             */
            void tryLogin(const std::string& username, const std::string& password) const;

            /**
             * @brief Send a request to server attempting to register with provided credentials
             * @param sessionId ID of the network session
             * @param username Username string
             * @param password Password string
             */
            void tryRegister(const std::string& username, const std::string& password) const;

            /**
             * @brief Send a request to have the list of available rooms from the server
             * @note It updates the internal _availableRooms list
             */
            void requestListRooms(void);

            /**
             * @brief Send a request to create a new room on the server
             * @param roomName Name of the room
             * @param maxPlayers Maximum number of players allowed
             * @param difficulty Difficulty level of the room
             * @param speed Speed multiplier for the game
             * @param duration Duration of the game session
             * @param seed Seed for random generation
             * @param levelId Level identifier
             */
            void tryCreateRoom(const std::string& roomName, uint32_t maxPlayers, float difficulty, float speed, uint32_t duration, uint32_t seed, uint32_t levelId);

            /**
             * @brief Send a request to join an existing room on the server
             * @param roomId Identifier of the room to join
             */
            void tryJoinRoom(uint32_t roomId);

            /**
             * @brief Send a request to leave the current room on the server
             */
            void tryLeaveRoom(void);

            /**
             * @brief Send a request to set the client's ready status in the current room
             * @param isReady true to set as ready, false to set as not ready
             */
            void trySetReady(bool isReady);

            /**
             * @brief Send a chat message to the server
             * @param message The message string to send
             */
            void trySendMessage(const std::string& message) const;
        
        public:
            /**
             * @brief Check if the client is in a room
             * @return true if in a room, false otherwise
             */
            bool isInRoom(void) const;

            /**
             * @brief Check if the client is ready
             * @return true if ready, false otherwise
             */
            bool isReady(void) const;

            /**
             * @brief Check if UDP is ready
             * @return true if UDP is ready, false otherwise
             */
            bool isUdpReady(void) const;

            /**
             * @brief Check if the client is logged in
             * @return true if logged in, false otherwise
             */
            bool isLoggedIn(void) const;

            /**
             * @brief Get the username of the client
             * @return The username string
             */
            std::string getUsername(void) const;

            /**
             * @brief Get a list of available rooms from the server
             * @return List of RoomInfo structures
             */
            std::list<rtp::net::RoomInfo> getAvailableRooms(void) const;

            /**
             * @brief Check if the client is in game
             * @return true if in game, false otherwise
             */
            bool isInGame(void) const;

        private:
            ClientNetwork& _network;                                       /**< Reference to the client network manager */
            rtp::ecs::Registry& _registry;                                 /**< Reference to the entity registry */
            std::unordered_map<uint32_t, rtp::ecs::Entity> _netIdToEntity; /**< Map of network IDs to entities */
            Client::Game::EntityBuilder _builder;                          /**< Entity builder for spawning entities */

        private:
            bool _isInRoom = false;                                        /**< Flag indicating if the client is in a room */
            bool _isReady = false;                                         /**< Flag indicating if the client is ready */
            bool _udpReady = false;                                        /**< Flag indicating if UDP is ready */
            bool _isLoggedIn = false;                                      /**< Flag indicating if the client is logged in */
            std::string _username;                                         /**< The username of the client */
            std::list<rtp::net::RoomInfo> _availableRooms;                 /**< List of available rooms from the server */
            State _currentState = State::NotLogged;                        /**< Current state of the client */
        
        private:
            /**
             * @brief Handle a network event
             * @param event Reference to the network event to handle
             */
            void handleEvent(rtp::net::NetworkEvent& event);

            void onLoginResponse(rtp::net::Packet& packet);

            void onRegisterResponse(rtp::net::Packet& packet);

            void onRoomResponse(rtp::net::Packet& packet);

            void onJoinRoomResponse(rtp::net::Packet& packet);

            void onCreateRoomResponse(rtp::net::Packet& packet);

            void onLeaveRoomResponse(rtp::net::Packet& packet);

            void onSpawnEntityFromServer(rtp::net::Packet& packet);

            void onRoomUpdate(rtp::net::Packet& packet);

            // /**
            //  * @brief Disconnect a player based on session ID
            //  * @param sessionId ID of the session to disconnect
            //  */
            // void disconnectPlayer(uint32_t sessionId);

            // /**
            //  * @brief Apply a world update from a network packet
            //  * @param packet Reference to the network packet containing the world update
            //  */
            // void applyWorldUpdate(rtp::net::Packet& packet);

            // /**
            //  * @brief Spawn a new entity based on a snapshot payload
            //  * @param snap Reference to the entity snapshot payload
            //  */
            // void spawnEntity(const rtp::net::EntitySnapshotPayload& snap);

            // /**
            //  * @brief Spawn an entity based on server spawn payload
            //  * @param payload Reference to the entity spawn payload
            //  */
            // void spawnEntityFromServer(const rtp::net::EntitySpawnPayload& payload);

            // /**
            //  * @brief Add sprite component for scout entity
            //  * @param entity Entity to which the sprite component will be added
            //  */
            // void addScoutSprite(rtp::ecs::Entity entity);
    };
}

#endif /* !RTYPE_CLIENT_NETWORK_SYSTEM_HPP_ */