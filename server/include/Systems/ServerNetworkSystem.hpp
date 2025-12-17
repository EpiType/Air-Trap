/**
 * File   : INetwork.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_NETWORK_SERVERNETWORKSYSTEM_HPP_
    #define RTYPE_NETWORK_SERVERNETWORKSYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"
    
    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/Network/Packet.hpp"

    #include <vector>
    #include <unordered_map>

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {

    /**
     * @class ServerNetworkSystem
     * @brief System to handle network-related operations on the server side.
     */
    class ServerNetworkSystem : public rtp::ecs::ISystem {
        public:
            /** 
             * @brief Constructor for ServerNetworkSystem
             * @param network Reference to the server network manager
             * @param registry Reference to the entity registry
             */
            ServerNetworkSystem(ServerNetwork& network, rtp::ecs::Registry& registry);

            /**
             * @brief Update system logic for one frame
             * @param dt Time elapsed since last update in seconds
             * @note Currently not used
             */
            void update(float dt) override;

            /**
             * @brief Broadcast the current world state to all connected clients
             * @param serverTick Current server tick count
             */
            void broadcastWorldState(uint32_t serverTick);

            /**
             * @brief Bind a network session to an entity
             * @param sessionId ID of the network session
             * @param entityId ID of the entity to bind
             */
            void bindSessionToEntity(uint32_t sessionId, uint32_t entityId);

            /**
             * @brief Handle input received from a client
             * @param sessionId ID of the network session
             * @param packet Packet containing the input data
             */
            void handleInput(uint32_t sessionId, const rtp::net::Packet& packet);
            
        private:
            ServerNetwork& _network;           /**< Reference to the server network manager */
            rtp::ecs::Registry& _registry;     /**< Reference to the entity registry */
            std::unordered_map<uint32_t,
                uint32_t> _sessionToEntity;    /**< Map of session IDs to entity IDs */
    };
}

#endif /* !RTYPE_NETWORK_SERVERNETWORKSYSTEM_HPP_ */