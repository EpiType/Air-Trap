/**
 * File   : NetworkBootstrap.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_NETWORKBOOTSTRAP_HPP_
    #define RTYPE_CLIENT_NETWORKBOOTSTRAP_HPP_

    /* Engine */
    #include "engine/net/INetwork.hpp"
    #include "engine/net/INetworkEngine.hpp"
    #include "engine/net/NetworkUtils.hpp"

    /* R-Type Shared */
    #include "rtype/net/Packet.hpp"

    #include <cstdint>
    #include <optional>
    #include <string>
    #include <vector>

namespace rtp::client {
    /**
     * @class NetworkBootstrap
     * @brief Bootstrap class for initializing the client network.
     * 
     * This class wraps the engine network plugin to provide a simple
     * client-facing INetwork implementation for R-Type.
     */
    class NetworkBootstrap : public engine::net::INetwork {
        public:
            /**
             * @brief Constructor for NetworkBootstrap
             * @param engine Network engine plugin to use
             * @param config Client configuration
             */
            NetworkBootstrap(engine::net::INetworkEngine &engine,
                             engine::net::ClientConfig config);

            /**
             * @brief Destructor for NetworkBootstrap
             */
            ~NetworkBootstrap() override;

            /**
             * @brief Start the network service.
             * @return true if the network started successfully, false otherwise
             */
            bool start(void) override;

            /**
             * @brief Stop the network service.
             */
            void stop(void) override;

        public:
            /**
             * @brief Send a packet to a specific session.
             * @param sessionId The ID of the session to send the packet to
             * @param payload The data payload to send
             * @param channel The network channel to use for sending
             */
            void sendPacket(uint32_t sessionId,
                            engine::net::ByteSpan payload,
                            engine::net::NetChannel channel) override;

            /**
             * @brief Poll for an incoming network event.
             * @return An optional network event
             */
            std::optional<engine::net::NetworkEvent> pollEvent(void) override;

            /**
             * @brief Send a packet using the specified network mode.
             * @param packet The packet to send
             * @param mode The network channel to use for sending
             */
            void sendPacket(const net::Packet &packet, engine::net::NetChannel mode);

        private:
            engine::net::INetworkEngine &_engine;       /**< Network engine plugin */
            engine::net::ClientConfig _config;          /**< Client configuration */
            engine::net::INetwork *_network{nullptr};   /**< Active network instance */
    };
} // namespace rtp::client

#endif /* !RTYPE_CLIENT_NETWORKBOOTSTRAP_HPP_ */
