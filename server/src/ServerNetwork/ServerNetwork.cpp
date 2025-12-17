/**
 * File   : INetwork.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "ServerNetwork/ServerNetwork.hpp"
#include "RType/Logger.hpp"

namespace rtp::server {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    ServerNetwork::ServerNetwork(uint16_t port)
        : _ioContext(),
          _acceptor(_ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)),
          _udpSocket(_ioContext, asio::ip::udp::endpoint(asio::ip::udp::v4(), port)),
          _nextSessionId(1)
    {
        rtp::log::info("ServerNetwork initialized on port {}", port);
    }

    ServerNetwork::~ServerNetwork()
    {
        stop();
    }

    void ServerNetwork::start()
    {
        rtp::log::info("Starting ServerNetwork...");
        
        acceptConnection();
        receiveUdpPacket();

        _ioThread = std::thread([this]() {
            try {
                rtp::log::info("ASIO Context running...");
                auto workGuard = asio::make_work_guard(_ioContext);
                _ioContext.run();
            } catch (const std::exception& e) {
                rtp::log::error("ServerNetwork context error: {}", e.what());
            }
        });
    }

    void ServerNetwork::stop()
    {
        if (!_ioContext.stopped()) {
            _ioContext.stop();
        }
        if (_ioThread.joinable()) {
            _ioThread.join();
        }
        _acceptor.close();
        _udpSocket.close();
        rtp::log::info("ServerNetwork stopped.");
    }

    void ServerNetwork::sendPacket(uint32_t sessionId, const rtp::net::Packet& packet, rtp::net::NetworkMode mode)
    {
        std::lock_guard<std::mutex> lock(_sessionsMutex);
        auto it = _sessions.find(sessionId);
        if (it != _sessions.end()) {
            it->second->send(packet, mode);
        }
    }

    void ServerNetwork::broadcastPacket(const rtp::net::Packet& packet, rtp::net::NetworkMode mode)
    {
        std::lock_guard<std::mutex> lock(_sessionsMutex);
        for (auto& [id, session] : _sessions) {
            session->send(packet, mode);
        }
    }

    std::optional<rtp::net::NetworkEvent> ServerNetwork::pollEvent()
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);
        if (_eventQueue.empty()) {
            return std::nullopt;
        }
        auto event = _eventQueue.front();
        _eventQueue.pop();
        return event;
    }

    void ServerNetwork::publishEvent(rtp::net::NetworkEvent event)
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);
        _eventQueue.push(event);
    }

    //////////////////////////////////////////////////////////////////////////
    // Private Implementation
    //////////////////////////////////////////////////////////////////////////

    void ServerNetwork::acceptConnection()
    {
        auto newSocket = std::make_shared<asio::ip::tcp::socket>(_ioContext);

        _acceptor.async_accept(*newSocket, [this, newSocket](const asio::error_code& error) {
            if (!error) {
                uint32_t id = _nextSessionId++;
                rtp::log::info("New connection accepted from {}. Session ID: {}", 
                    newSocket->remote_endpoint().address().to_string(), id);

                auto session = std::make_shared<rtp::net::Session>(
                    id, 
                    std::move(*newSocket), 
                    _udpSocket, 
                    *this
                );

                {
                    std::lock_guard<std::mutex> lock(_sessionsMutex);
                    _sessions[id] = session;
                }

                session->start();
                
                rtp::net::Packet welcome(rtp::net::OpCode::Welcome);
                welcome << id;
                session->send(welcome, rtp::net::NetworkMode::TCP);

            } else {
                rtp::log::error("Accept error: {}", error.message());
            }

            acceptConnection();
        });
    }

    void ServerNetwork::receiveUdpPacket()
{
    _udpSocket.async_receive_from(
        asio::buffer(_udpBuffer),
        _udpRemoteEndpoint,
        [this](const asio::error_code& error, std::size_t bytesTransferred)
        {
            if (error) {
                if (error != asio::error::operation_aborted)
                    rtp::log::error("UDP receive error: {}", error.message());
                receiveUdpPacket();
                return;
            }

            if (bytesTransferred < sizeof(rtp::net::Header)) {
                receiveUdpPacket();
                return;
            }

            rtp::net::Header header;
            std::memcpy(&header, _udpBuffer.data(), sizeof(header));

            header.magic      = rtp::net::Packet::from_network(header.magic);
            header.sequenceId = rtp::net::Packet::from_network(header.sequenceId);
            header.bodySize   = rtp::net::Packet::from_network(header.bodySize);
            header.ackId      = rtp::net::Packet::from_network(header.ackId);
            header.sessionId  = rtp::net::Packet::from_network(header.sessionId);

            if (header.magic != rtp::net::MAGIC_NUMBER) {
                receiveUdpPacket();
                return;
            }

            if (bytesTransferred != sizeof(rtp::net::Header) + header.bodySize) {
                rtp::log::error("Malformed UDP packet (size mismatch) bytes={} expected={}",
                                bytesTransferred, sizeof(rtp::net::Header) + header.bodySize);
                receiveUdpPacket();
                return;
            }

            if (header.opCode == rtp::net::OpCode::Hello) {
                uint32_t claimedSessionId = header.sessionId;

                {
                    std::lock_guard<std::mutex> lock(_sessionsMutex);
                    auto it = _sessions.find(claimedSessionId);
                    if (it != _sessions.end()) {
                        it->second->setUdpEndpoint(_udpRemoteEndpoint);

                        {
                            std::lock_guard<std::mutex> udpLock(_udpMapMutex);
                            _udpEndpointToSessionId[_udpRemoteEndpoint] = claimedSessionId;
                        }

                        rtp::log::info("UDP bound: session {} -> {}:{}",
                                       claimedSessionId,
                                       _udpRemoteEndpoint.address().to_string(),
                                       _udpRemoteEndpoint.port());
                    }
                }

                receiveUdpPacket();
                return;
            }

            uint32_t realSessionId = 0;
            {
                std::lock_guard<std::mutex> lock(_udpMapMutex);
                auto it = _udpEndpointToSessionId.find(_udpRemoteEndpoint);
                if (it == _udpEndpointToSessionId.end()) {
                    receiveUdpPacket();
                    return;
                }
                realSessionId = it->second;
            }

            rtp::net::Packet packet;
            header.sessionId = realSessionId;
            packet.header = header;

            if (header.bodySize > 0) {
                packet.body.resize(header.bodySize);
                std::memcpy(packet.body.data(),
                            _udpBuffer.data() + sizeof(rtp::net::Header),
                            header.bodySize);
            }

            publishEvent({ realSessionId, packet });

            receiveUdpPacket();
        }
    );
}


} // namespace rtp::server