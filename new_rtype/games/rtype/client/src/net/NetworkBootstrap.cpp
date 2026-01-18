/**
 * File   : NetworkBootstrap.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/net/NetworkBootstrap.hpp"
#include "engine/log/Logger.hpp"

namespace rtp::client
{
    NetworkBootstrap::NetworkBootstrap(aer::net::INetworkEngine &engine,
                                       aer::net::ClientConfig config)
        : _engine(engine),
          _config(std::move(config))
    {
    }

    /////////////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////////////

    NetworkBootstrap::~NetworkBootstrap()
    {
        stop();
    }

    bool NetworkBootstrap::start(void)
    {
        if (_network) {
            return _network->start();
        }
        aer::log::info("NetworkBootstrap: Creating network client");
        _network = _engine.createClient(_config);
        if (!_network) {
            return false;
        }
        aer::log::info("NetworkBootstrap: Starting network client");
        return _network->start();
    }

    void NetworkBootstrap::stop(void)
    {
        if (_network) {
            _network->stop();
            _engine.destroy(_network);
            _network = nullptr;
        }
    }

    ////////////////////////////////////////////////////////////////////////
    // Private API
    ////////////////////////////////////////////////////////////////////////

    void NetworkBootstrap::sendPacket(uint32_t sessionId,
                                      aer::net::ByteSpan payload,
                                      aer::net::NetChannel channel)
    {
        if (_network) {
            _network->sendPacket(sessionId, payload, channel);
        }
    }

    void NetworkBootstrap::sendPacket(const net::Packet &packet, aer::net::NetChannel mode)
    {
        const auto data = packet.serialize();
        sendPacket(packet.header.sessionId, data, mode);
    }

    std::optional<aer::net::NetworkEvent> NetworkBootstrap::pollEvent(void)
    {
        if (!_network) {
            return std::nullopt;
        }
        return _network->pollEvent();
    }
} // namespace rtp::client
