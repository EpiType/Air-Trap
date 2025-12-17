/**
 * File   : ClientNetworkSystem.hpp
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

    #include <vector>
    #include <iostream>

/**
 * @namespace rtp::client
 * @brief Systems for R-Type client
 */
namespace rtp::client {

    /**
     * @class ClientNetworkSystem
     * @brief System to handle network-related operations on the client side.
     */
    class ClientNetworkSystem : public rtp::ecs::ISystem
    {
        public:
            /** 
             * @brief Constructor for ClientNetworkSystem
             * @param network Reference to the client network manager
             * @param registry Reference to the entity registry
             */
            ClientNetworkSystem(ClientNetwork& network, rtp::ecs::Registry& registry);

            /**
             * @brief Update system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            ClientNetwork& _network;         /**< Reference to the client network manager */
            rtp::ecs::Registry& _registry;   /**< Reference to the entity registry */
        
        private:
            /**
             * @brief Handle a network event
             * @param event Reference to the network event to handle
             */
            void handleEvent(rtp::net::NetworkEvent& event);

            /**
             * @brief Apply a world update from a network packet
             * @param packet Reference to the network packet containing the world update
             */
            void applyWorldUpdate(rtp::net::Packet& packet);

            /**
             * @brief Spawn a new entity based on a snapshot payload
             * @param snap Reference to the entity snapshot payload
             */
            void spawnEntity(const rtp::net::EntitySnapshotPayload& snap);
    };
}

#endif /* !RTYPE_CLIENT_NETWORK_SYSTEM_HPP_ */