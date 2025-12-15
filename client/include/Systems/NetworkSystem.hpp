/**
 * File   : NetworkSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_NETWORK_SYSTEM_HPP_
    #define RTYPE_NETWORK_SYSTEM_HPP_

    #include "RType/ECS/Registry.hpp"
    #include "RType/Logger.hpp"
    #include "RType/ECS/ISystem.hpp"
    #include "include/Network/ClientNetwork.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Sprite.hpp"

namespace rtp::client {

    class NetworkSystem : public rtp::ecs::ISystem {
        public:
            NetworkSystem(rtp::ecs::Registry& r, rtp::client::ClientNetwork& network) 
                : _r(r), _network(network) {}

            void update(float dt) override {
                // todo
            }
            
        private:
            rtp::ecs::Registry& _r;
            rtp::client::ClientNetwork& _network;
    };
}  // namespace rtp::client

#endif /* !RTYPE_NETWORK_SYSTEM_HPP_ */