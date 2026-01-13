/**
 * File   : AuthSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_LOGIN_SYSTEM_HPP_
    #define RTYPE_LOGIN_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"
    
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/Network/Packet.hpp"

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */

namespace rtp::server {

    /**
     * @class AuthSystem
     * @brief System to handle player login operations on the server side.
     */
    class AuthSystem : public rtp::ecs::ISystem {
        public:
            /** 
             * @brief Constructor for AuthSystem
             * @param network Reference to the server network manager
             * @param registry Reference to the entity registry
             */
            AuthSystem(ServerNetwork& network, rtp::ecs::Registry& registry);

            /**
             * @brief Update system logic for one frame
             * @param dt Time elapsed since last update in seconds
             * @note Currently not used
             */
            void update(float dt) override;

            /**
             * @brief Handle a login request from a client
             * @param sessionId ID of the network session
             * @param packet Packet containing the login request data
             */
            std::pair<bool, std::string> handleLoginRequest(uint32_t sessionId, const rtp::net::Packet& packet);

            /**
             * @brief Handle a registration request from a client
             * @param sessionId ID of the network session
             * @param packet Packet containing the registration request data
             */
            std::pair<bool, std::string> handleRegisterRequest(uint32_t sessionId, const rtp::net::Packet& packet);

            /**
             * @brief Send a login response to a client
             * @param sessionId ID of the network session
             * @param success Whether the login was successful
             * @param username The username of the player
             */
            void sendLoginResponse(uint32_t sessionId, bool success, const std::string& username);

            /**
             * @brief Send a registration response to a client
             * @param sessionId ID of the network session
             * @param success Whether the registration was successful
             * @param username The username of the player
             */
            void sendRegisterResponse(uint32_t sessionId, bool success, const std::string& username);

        private:
            ServerNetwork& _network;          /**< Reference to the server network manager */
            rtp::ecs::Registry& _registry;    /**< Reference to the entity registry */
    };
}

#endif /* !RTYPE_LOGIN_SYSTEM_HPP_ */