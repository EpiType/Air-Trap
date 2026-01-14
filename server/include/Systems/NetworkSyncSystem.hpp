/**
 * File   : NetworkSyncSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_NETWORK_NetworkSyncSystem_HPP_
    #define RTYPE_NETWORK_NetworkSyncSystem_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"
    
    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/ECS/Components/RoomId.hpp"
    #include "RType/Network/Packet.hpp"

    #include <vector>
    #include <unordered_map>

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {

    /**
     * @class NetworkSyncSystem
     * @brief System to handle network-related operations on the server side.
     */
    class NetworkSyncSystem : public rtp::ecs::ISystem {
        public:
            /** 
             * @brief Constructor for NetworkSyncSystem
             * @param network Reference to the server network manager
             * @param registry Reference to the entity registry
             */
            NetworkSyncSystem(ServerNetwork& network, rtp::ecs::Registry& registry);

            /**
             * @brief Update system logic for one frame
             * @param dt Time elapsed since last update in seconds
             * @note Currently not used
             */
            void update(float dt) override;

            /**
             * @brief Bind a network session to an entity
             * @param sessionId ID of the network session
             * @param entityId ID of the entity to bind
             */
            void bindSessionToEntity(uint32_t sessionId, uint32_t entityId);
            void unbindSession(uint32_t sessionId);

            /**
             * @brief Handle input received from a client
             * @param sessionId ID of the network session
             * @param packet Packet containing the input data
             */
            void handleInput(uint32_t sessionId, const rtp::net::Packet& packet);

            /**
             * @brief Handle disconnection of a client
             * @param sessionId ID of the disconnected session
             */
            void handleDisconnect(uint32_t sessionId);

            /**
             * @brief Handle new player connection
             * @param sessionId ID of the new session
             * @param packet Packet containing the connection data
             */
            uint32_t handlePlayerConnection(uint32_t sessionId, const rtp::net::Packet& packet);

            /**
             * @brief Send a packet to the entity associated with the given ID
             * @param entityId ID of the target entity
             * @param packet Packet to send
             * @param mode Network mode (TCP or UDP)
             */
            void sendPacketToEntity(uint32_t entityId, const rtp::net::Packet& packet, rtp::net::NetworkMode mode);

            /**
             * @brief Send a packet to a specific session
             * @param sessionId ID of the target session
             * @param packet Packet to send
             * @param mode Network mode (TCP or UDP)
             */
            void sendPacketToSession(uint32_t sessionId, const rtp::net::Packet& packet, rtp::net::NetworkMode mode);

            /**
             * @brief Send a packet to multiple sessions
             * @param sessions List of session IDs
             * @param packet Packet to send
             * @param mode Network mode (TCP or UDP)
             */
            void sendPacketToSessions(const std::vector<uint32_t>& sessions, const rtp::net::Packet& packet, rtp::net::NetworkMode mode);
            
        private:
            ServerNetwork& _network;           /**< Reference to the server network manager */
            rtp::ecs::Registry& _registry;     /**< Reference to the entity registry */
            std::unordered_map<uint32_t,
                uint32_t> _sessionToEntity;    /**< Map of session IDs to entity IDs */
    };
}

#endif /* !RTYPE_NETWORK_NetworkSyncSystem_HPP_ */
