/**
 * File   : NetworkBootstrap.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_SERVER_NETWORKBOOTSTRAP_HPP_
    #define RTYPE_SERVER_NETWORKBOOTSTRAP_HPP_

    /* Engine */
    #include "engine/net/INetwork.hpp"
    #include "engine/net/INetworkEngine.hpp"
    #include "engine/net/NetworkUtils.hpp"

    /* R-Type Shared */
    #include "rtype/net/Packet.hpp"

    #include <optional>

namespace rtp::server::net
{
    /**
     * @class NetworkBootstrap
     * @brief Bootstrap class for initializing the server network.
     * @details This class sets up the network engine and wraps a server
     * INetwork instance for the R-Type server.
     */
    class NetworkBootstrap : public aer::net::INetwork {
        public:
            /**
             * @brief Constructor for NetworkBootstrap
             * @param networkEngine Pointer to the network engine implementation
             */
            explicit NetworkBootstrap(aer::net::INetworkEngine &networkEngine,
                                      aer::net::ServerConfig config);

            /**
             * @brief Destructor for NetworkBootstrap
             */
            ~NetworkBootstrap() override;

            /**
             * @brief Start the network server
             * @return True if the server started successfully, false otherwise
             */
            bool start(void) override;

            /**
             * @brief Stop the network server
             */
            void stop(void) override;

            /**
             * @brief Send a raw payload to a session.
             * @param sessionId Session identifier
             * @param payload Payload to send
             * @param channel Network channel
             */
            void sendPacket(uint32_t sessionId,
                            aer::net::ByteSpan payload,
                            aer::net::NetChannel channel) override;

            /**
             * @brief Poll for incoming network events.
             * @return Optional network event
             */
            std::optional<aer::net::NetworkEvent> pollEvent(void) override;

            /**
             * @brief Send a packet to a specific session.
             * @param packet Packet to send
             * @param channel Network channel
             */
            void sendPacket(const rtp::net::Packet &packet, aer::net::NetChannel channel);

        private:
            aer::net::INetworkEngine &_networkEngine; /**< Network engine instance */
            aer::net::ServerConfig _config;           /**< Server configuration */
            aer::net::INetwork *_network{nullptr};    /**< Network instance */
    };

} // namespace rtp::server::net

#endif /* !RTYPE_SERVER_NETWORKBOOTSTRAP_HPP_ */