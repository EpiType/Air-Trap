/**
 * File   : Network.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Network/Network.hpp"

namespace rtp::server::net {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    Network::Network(uint16_t port)
        : 
    _tcpAcceptor(_ioContext),
    _udpSocket(_ioContext),
    _nextSessionId(1),
    _eventQueue(),
    _ioThread(),
    _port(port)
    {
        asio::ip::tcp::endpoint tcpEndpoint(asio::ip::tcp::v4(), port);
        _tcpAcceptor.open(tcpEndpoint.protocol());
        _tcpAcceptor.set_option(asio::socket_base::reuse_address(true));
        _tcpAcceptor.bind(tcpEndpoint);

        asio::ip::udp::endpoint udpEndpoint(asio::ip::udp::v4(), port);
        _udpSocket.open(udpEndpoint.protocol());
        _udpSocket.set_option(asio::socket_base::reuse_address(true));
        _udpSocket.bind(udpEndpoint);  
    };

    Network::~Network()
    {
        this->stop();
    }

    void Network::start(void)
    {
        /* Run the TCP acceptor coroutine */
        try {
            _tcpAcceptor.listen();
            asio::co_spawn(
                _ioContext,
                [this]() { return this->runTcpAcceptor(); },
                asio::detached
            );
            log::info("TCP Acceptor started on port {}", _port);
        }
        catch (const std::exception &e) {
            log::error("Failed to start TCP acceptor: {}", e.what());
            return stop();
        }

        /* Run the UDP acceptor coroutine */
        try {
            asio::co_spawn(
                _ioContext,
                [this]() { return this->runUdpReader(); },
                asio::detached
            );
            log::info("UDP Listener started on port {}", _port);
        } catch (const asio::system_error& e) {
            log::error("Failed to start UDP listener: {}", e.what());
            return stop();
        }

        /* Start the ASIO I/O context in a separate thread */
        _ioThread = std::thread([this]() {
            log::info("ASIO I/O thread started.");
            try {
                _ioContext.run();
            } catch (const std::exception& e) {
                log::fatal("ASIO I/O context crashed: {}", e.what());
            }
        });
    }

    void Network::stop(void)
    {
        _ioContext.stop();
        if (_ioThread.joinable()) {
            _ioThread.join();
            log::info("ASIO I/O thread stopped.");
        }

        asio::error_code ec;

        _tcpAcceptor.close(ec);
        if (ec) {
            log::error("Error closing TCP acceptor: {}", ec.message());
        }

        _udpSocket.close(ec);
        if (ec) {
            log::error("Error closing UDP socket: {}", ec.message());
        }

        log::info("Server network successfully stopped.");
    }

    void Network::sendPacket(uint32_t sessionId, const Packet &packet, NetworkMode mode)
    {
        std::lock_guard<std::mutex> lock(_sessionsMutex);

        if (auto it = _sessions.find(sessionId); it != _sessions.end()) {
            auto session = it->second;
            
            if (mode == NetworkMode::TCP) {
                session->sendTcp(packet);
            } else if (mode == NetworkMode::UDP) {
                if (session->hasUdp()) {
                    this->sendUdpHelper(session->getUdpEndpoint(), packet);
                } else {
                    log::warning("Attempted to send UDP packet to Session ID {} but no UDP endpoint is set.", sessionId);
                }
            }
        } else {
            log::warning("Attempted to send packet to non-existent Session ID {}", sessionId);
        }
    }

    void Network::broadcastPacket(const Packet &packet, NetworkMode mode)
    {
        std::lock_guard<std::mutex> lock(_sessionsMutex);

        for (auto const& [id, session] : _sessions) {
            if (mode == NetworkMode::TCP) {
                session->sendTcp(packet);
            } else if (mode == NetworkMode::UDP) {
                if (session->hasUdp()) {
                    this->sendUdpHelper(session->getUdpEndpoint(), packet);
                }
            }
        }
    }

    std::optional<NetworkEvent> Network::pollEvent(void)
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);

        if (_eventQueue.empty()) { return std::nullopt; }

        NetworkEvent event = std::move(_eventQueue.front());
        _eventQueue.pop();

        return event;
    }

    void Network::publishEvent(NetworkEvent event)
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);
        
        _eventQueue.push(std::move(event));
    }

    uint32_t Network::getNextSessionId(void)
    {
        std::lock_guard<std::mutex> lock(_sessionsMutex);
        return _nextSessionId++;
    }

    //////////////////////////////////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////////////////////////////////

    asio::awaitable<void> Network::runTcpAcceptor()
    {
        for (;;)
        {
            try {
                asio::ip::tcp::socket socket = co_await _tcpAcceptor.async_accept(asio::use_awaitable);

                uint32_t sessionId = this->getNextSessionId();
                
                auto session = std::make_shared<rtp::net::Session>(std::move(socket), *this);

                session->setId(sessionId);
                {
                    std::lock_guard<std::mutex> lock(_sessionsMutex);
                    _sessions.emplace(sessionId, session);
                }
                session->start();

                publishEvent({sessionId, Packet(OpCode::Hello)});

                log::info("New TCP connection accepted. Assigned Session ID {}", sessionId);
            } catch (const asio::system_error& e) {
                log::error("Error accepting TCP connection: {}", e.what());
            } catch (const std::exception& e) {
                log::fatal("Unexpected error in TCP acceptor: {}", e.what());
            }
        }
    }

    asio::awaitable<void> Network::runUdpReader()
    {
        char recvBuffer[65536];
        asio::ip::udp::endpoint remoteEndpoint;

        for (;;)
        {
            size_t bytesReceived = co_await _udpSocket.async_receive_from(
                asio::buffer(recvBuffer, sizeof(recvBuffer)),
                remoteEndpoint,
                asio::use_awaitable
            );

            rtp::net::Header header;
            if (bytesReceived < sizeof(header)) {
                log::warning("Received UDP packet too small from {}", remoteEndpoint.address().to_string());
                continue;
            }

            std::memcpy(&header, recvBuffer, sizeof(header));

            if (header.magic != rtp::net::MAGIC_NUMBER) {
                log::warning("Invalid magic number in UDP packet from {}", remoteEndpoint.address().to_string());
                continue;
            }

            rtp::net::Packet packet;
            packet.header = header;
            packet.body.resize(header.bodySize);
            std::memcpy(packet.body.data(), recvBuffer + sizeof(header), header.bodySize);

            uint32_t sessionId = 0;
            {
                std::lock_guard<std::mutex> lock(_sessionsMutex);
                auto it = _udpEndpointToId.find(remoteEndpoint);
                if (it != _udpEndpointToId.end()) {
                    sessionId = it->second;
                }
            }

            if (sessionId != 0) {
                rtp::net::NetworkEvent event;
                event.sessionId = sessionId;
                event.packet = std::move(packet);
                this->publishEvent(std::move(event));
            } else {
                log::warning("Received UDP packet from unknown endpoint {}", remoteEndpoint.address().to_string());
            }
        }
    }

    void Network::sendUdpHelper(const asio::ip::udp::endpoint &endpoint, const Packet &packet)
    {
        try {
            auto buffers = packet.getBufferSequence();

            _udpSocket.send_to(buffers, endpoint);            
        }
        catch (const asio::system_error& e) {
            log::error("UDP send error to {}:{}: {}", 
                        endpoint.address().to_string(), endpoint.port(), e.what());
        }
    }

} // namespace rtp::server::net