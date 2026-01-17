/**
 * File   : ClientNetworkSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_CLIENT_NETWORK_SYSTEM_HPP_
    #define RTYPE_CLIENT_NETWORK_SYSTEM_HPP_

    #include <list>
    #include <string>
    #include <unordered_map>

    /* Engine Netowrk */
    #include "engine/ecs/ISystem.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/net/INetwork.hpp"
    #include "engine/net/NetworkUtils.hpp"
    
    /* Engine ECS Components */
    #include "engine/ecs/components/ImportAllEngineComponents.hpp"

    /*R-Type ECS Components */
    #include "rtype/components/ParallaxLayer.hpp"

    /* R-Type Client */
    #include "rtype/entity/EntityBuilder.hpp"
    #include "rtype/Packet.hpp"
    

namespace rtp::client {

    /**
     * @class NetworkSyncSystem
     * @brief System to handle network-related operations on the client side.
     */
    class NetworkSyncSystem : public engine::ecs::ISystem
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
            NetworkSyncSystem(engine::net::INetwork& network,
                            engine::ecs::Registry& registry,
                            EntityBuilder builder);

            //////////////////////////////////////////////////////////////////////////////
            // Network call methods
            //////////////////////////////////////////////////////////////////////////////

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
            void tryJoinRoom(uint32_t roomId, bool asSpectator = false);

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

            //////////////////////////////////////////////////////////////////////////
            // Getters
            //////////////////////////////////////////////////////////////////////////

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

            /**
             * @brief Get the current state of the client
             * @return Current State enum value
             */
            State getState(void) const;

            /**
             * @brief Get the last received room chat message
             * @return Last chat message string
             */
            std::string getLastChatMessage(void) const;

            /**
             * @brief Get chat history buffer (most recent last)
             * @return Chat message buffer
             */
            const std::deque<std::string>& getChatHistory(void) const;

            /**
             * @brief Get current ammo status
             * @return Current ammo, max ammo, reloading status, and cooldown remaining
             */
            uint16_t getAmmoCurrent(void) const;

            /**
             * @brief Get maximum ammo capacity
             * @return Maximum ammo
             */
            uint16_t getAmmoMax(void) const;

            /**
             * @brief Check if ammo is currently reloading
             * @return true if reloading, false otherwise
             */
            bool isReloading(void) const;

            /**
             * @brief Get remaining cooldown time for ammo reload
             * @return Cooldown time in seconds
             */
            float getReloadCooldownRemaining(void) const;

            /**
             * @brief Get the latest ping in milliseconds
             * @return Ping time in ms
             */
            uint32_t getPingMs(void) const;

            /**
             * @brief Consume and reset the kicked flag
             * @return true if the client was kicked, false otherwise
             */
            bool consumeKicked(void);

        private:
            engine::net::INetwork& _network;                                       /**< Reference to the client network manager */
            engine::ecs::Registry& _registry;                                 /**< Reference to the entity registry */
            std::unordered_map<uint32_t, engine::ecs::Entity> _netIdToEntity; /**< Map of network IDs to entities */
            EntityBuilder _builder;                                        /**< Entity builder for spawning entities */

        private:
            bool _isInRoom = false;                                        /**< Flag indicating if the client is in a room */
            bool _isReady = false;                                         /**< Flag indicating if the client is ready */
            bool _udpReady = false;                                        /**< Flag indicating if UDP is ready */
            bool _isLoggedIn = false;                                      /**< Flag indicating if the client is logged in */
            std::string _username;                                         /**< The username of the client */
            std::list<rtp::net::RoomInfo> _availableRooms;                 /**< List of available rooms from the server */
            State _currentState = State::NotLogged;                        /**< Current state of the client */
            std::string _lastChatMessage;                                  /**< Last received chat message */
            std::deque<std::string> _chatHistory;                          /**< Recent chat messages */
            std::size_t _chatHistoryLimit = 8;                             /**< Max chat messages to keep */
            uint16_t _ammoCurrent = 0;                                     /**< Current ammo */
            uint16_t _ammoMax = 100;                                       /**< Max ammo */
            bool _ammoReloading = false;                                   /**< Reload state */
            float _ammoReloadRemaining = 0.0f;                             /**< Reload time remaining */
            uint32_t _pingMs = 0;                                          /**< Latest ping in ms */
            float _pingTimer = 0.0f;                                       /**< Ping timer accumulator */
            float _pingInterval = 1.0f;                                    /**< Ping interval in seconds */
            bool _kicked = false;                                          /**< Kicked flag */
        
        private:
            /**
             * @brief Handle a network event
             * @param event Reference to the network event to handle
             */
            void handleEvent(engine::net::NetworkEvent& event);

            //////////////////////////////////////////////////////////////////////////////
            // Packet handlers
            //////////////////////////////////////////////////////////////////////////////

            void onLoginResponse(rtp::net::Packet& packet);

            void onRegisterResponse(rtp::net::Packet& packet);

            void onRoomResponse(rtp::net::Packet& packet);

            void onJoinRoomResponse(rtp::net::Packet& packet);

            void onCreateRoomResponse(rtp::net::Packet& packet);

            void onLeaveRoomResponse(rtp::net::Packet& packet);

            void onSpawnEntityFromServer(rtp::net::Packet& packet);

            void onEntityDeath(rtp::net::Packet& packet);

            void onRoomUpdate(rtp::net::Packet& packet);

            void onRoomChatReceived(rtp::net::Packet& packet);

            void pushChatMessage(const std::string& message);

            void onAmmoUpdate(rtp::net::Packet& packet);

            void onPong(rtp::net::Packet& packet);

            void onDebugModeUpdate(rtp::net::Packet& packet);
    };
}

#endif // RTYPE_CLIENT_NETWORK_SYSTEM_HPP_
