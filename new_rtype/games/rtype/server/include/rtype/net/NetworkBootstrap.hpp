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
    class NetworkBootstrap : public engine::net::INetwork {
        public:
            /**
             * @brief Constructor for NetworkBootstrap
             * @param networkEngine Pointer to the network engine implementation
             */
            explicit NetworkBootstrap(engine::net::INetworkEngine &networkEngine,
                                      engine::net::ServerConfig config);

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
                            engine::net::ByteSpan payload,
                            engine::net::NetChannel channel) override;

            /**
             * @brief Poll for incoming network events.
             * @return Optional network event
             */
            std::optional<engine::net::NetworkEvent> pollEvent(void) override;

            /**
             * @brief Send a packet to a specific session.
             * @param packet Packet to send
             * @param channel Network channel
             */
            void sendPacket(const rtp::net::Packet &packet, engine::net::NetChannel channel);

        private:
            engine::net::INetworkEngine &_networkEngine; /**< Network engine instance */
            engine::net::ServerConfig _config;           /**< Server configuration */
            engine::net::INetwork *_network{nullptr};    /**< Network instance */
    };

} // namespace rtp::server::net

#endif /* !RTYPE_SERVER_NETWORKBOOTSTRAP_HPP_ */