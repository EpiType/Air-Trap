/**
 * File   : NetworkBootstrap.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "server/include/rtype/net/NetworkBootstrap.hpp"
#include "engine/core/Logger.hpp"

namespace rtp::server::net
{

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    NetworkBootstrap::NetworkBootstrap(engine::net::INetworkEngine &networkEngine,
                                       engine::net::ServerConfig config)
        : _networkEngine(networkEngine),
          _config(std::move(config))
    {
    }

    NetworkBootstrap::~NetworkBootstrap()
    {
        stop();
    }

    bool NetworkBootstrap::start(void)
    {
        if (_network) {
            engine::core::info("NetworkBootstrap: Network already started");
            return _network->start();
        }
        engine::core::info("NetworkBootstrap: Creating network server");
        _network = _networkEngine.createServer(_config);
        if (!_network) {
            return false;
        }
        engine::core::info("NetworkBootstrap: Starting network server");
        return _network->start();
    }

    void NetworkBootstrap::stop(void)
    {
        if (_network) {
            _network->stop();
            _networkEngine.destroy(_network);
            _network = nullptr;
        }
    }

    void NetworkBootstrap::sendPacket(uint32_t sessionId,
                                      engine::net::ByteSpan payload,
                                      engine::net::NetChannel channel)
    {
        if (_network) {
            _network->sendPacket(sessionId, payload, channel);
        }
    }

    std::optional<engine::net::NetworkEvent> NetworkBootstrap::pollEvent(void)
    {
        if (!_network) {
            return std::nullopt;
        }
        return _network->pollEvent();
    }

    void NetworkBootstrap::sendPacket(const rtp::net::Packet &packet,
                                      engine::net::NetChannel channel)
    {
        const auto data = packet.serialize();
        sendPacket(packet.header.sessionId, data, channel);
    }
} // namespace rtp::server::net
