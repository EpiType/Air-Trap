/**
 * File   : ClientNetworkSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_CLIENT_NETWORK_SYSTEM_HPP_
    #define RTYPE_CLIENT_NETWORK_SYSTEM_HPP_

    /* Engine Netowrk */
    #include "engine/ecs/ISystem.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/net/ANetwork.hpp"
    #include "engine/net/NetworkUtils.hpp"
    
    /* Engine ECS Components */
    #include "engine/ecs/components/Transform.hpp"
    #include "engine/ecs/components/Animation.hpp"
    #include "engine/ecs/components/Velocity.hpp"
    #include "engine/ecs/components/NetworkId.hpp"
    #include "engine/ecs/components/Hitbox.hpp"
    #include "engine/ecs/components/BoundingBox.hpp"
    #include "engine/ecs/components/Sprite.hpp"

    /* R-Type Client */
    // #include "RType/Network/Packet.hpp"
    #include "RType/ECS/Components/Sprite.hpp"
    // #include "Game/EntityBuilder.hpp"

    
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
            NetworkSyncSystem(engine::net::ANetwork& network, engine::ecs::Registry& registry, EntityBuilder builder);
    }