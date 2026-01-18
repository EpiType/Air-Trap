/**
 * File   : AuthSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_SERVER_AUTH_SYSTEM_HPP_
    #define RTYPE_SERVER_AUTH_SYSTEM_HPP_

    #include "engine/net/INetwork.hpp"
    #include "rtype/net/Packet.hpp"

    #include <string>
    #include <utility>

namespace rtp::server::systems
{
    /**
     * @class AuthSystem
     * @brief System for handling authentication (login and registration).
     */
    class AuthSystem {
        public:
            /**
             * @brief Constructor for AuthSystem
             * @param network Reference to the network instance
             */
            explicit AuthSystem(aer::net::INetwork &network);

            /**
             * @brief Handle a login request packet.
             * @param sessionId Session identifier of the client
             * @param packet Packet containing the login request
             * @return Pair of success status and username
             */
            std::pair<bool, std::string> handleLoginRequest(uint32_t sessionId,
                                                            const rtp::net::Packet &packet);

            /**
             * @brief Handle a registration request packet.
             * @param sessionId Session identifier of the client
             * @param packet Packet containing the registration request
             * @return Pair of success status and username
             */
            std::pair<bool, std::string> handleRegisterRequest(uint32_t sessionId,
                                                               const rtp::net::Packet &packet);

        private:
            /**
             * @brief Send a login response packet to the client.
             * @param sessionId Session identifier of the client
             * @param success True if login was successful, false otherwise
             * @param username Username of the client
             */
            void sendLoginResponse(uint32_t sessionId,
                                   bool success,
                                   const std::string &username);
            
            /**
             * @brief Send a registration response packet to the client.
             * @param sessionId Session identifier of the client
             * @param success True if registration was successful, false otherwise
             * @param username Username of the client
             */
            void sendRegisterResponse(uint32_t sessionId,
                                      bool success,
                                      const std::string &username);

        private:
            aer::net::INetwork &_network;        /**< Network instance for communication */
    };
}

#endif /* !RTYPE_SERVER_AUTH_SYSTEM_HPP_ */
