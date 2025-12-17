/**
 * File   : Network.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Network/ClientNetwork.hpp"
#include "RType/Logger.hpp"

namespace rtp::client {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    ClientNetwork::ClientNetwork(const std::string& serverIp, uint16_t serverPort)
        : _ioContext(),
          _tcpSocket(_ioContext),
          _udpSocket(_ioContext),
          _serverPort(serverPort),
          _serverIp(serverIp),
          _eventQueue(),
          _ioThread()
    {
    }

    ClientNetwork::~ClientNetwork()
    {
        this->stop();
    }

    void ClientNetwork::start(void)
    {
        try {
            _ioThread = std::thread([this]() {
                auto workGuard = asio::make_work_guard(_ioContext);
                _ioContext.run();
            });

            asio::error_code ec;
            asio::ip::tcp::endpoint endpoint(asio::ip::make_address(_serverIp, ec), _serverPort);
            _tcpSocket.connect(endpoint, ec);

            if (ec) {
                log::error("Error connecting to server: {}", ec.message());
                return this->stop();
            }

            this->sendPacket(net::Packet(net::OpCode::Hello), net::NetworkMode::TCP);
            log::info("Connected to server {}:{}", _serverIp, _serverPort);

            _udpSocket.open(asio::ip::udp::v4());
            _serverEndpoint = asio::ip::udp::endpoint(asio::ip::make_address(_serverIp), _serverPort);

            readTcpHeader();
            readUdp();

        } catch (const std::exception& e) {
            log::error("Client Network Start Error: {}", e.what());
            this->stop();
        }
    }

    void ClientNetwork::stop(void)
    {
        _ioContext.stop();

        if (std::this_thread::get_id() != _ioThread.get_id()) {
            if (_ioThread.joinable()) {
                _ioThread.join();
                log::info("ASIO I/O thread stopped.");
            }
        }

        if (_tcpSocket.is_open()) _tcpSocket.close();
        if (_udpSocket.is_open()) _udpSocket.close();
    };

    void ClientNetwork::sendPacket(const net::Packet &packet, net::NetworkMode mode)
    {
        asio::error_code ec;
        if (mode == net::NetworkMode::TCP) {
            asio::write(_tcpSocket, packet.getBufferSequence(), ec);
            if (ec) {
                log::error("Error sending TCP packet: {}", ec.message());
            }
        } else if (mode == net::NetworkMode::UDP) {
            if (_serverEndpoint.port() == 0) {
                asio::ip::udp::resolver resolver(_ioContext);
                _serverEndpoint = *resolver.resolve(asio::ip::udp::v4(), _serverIp, std::to_string(_serverPort)).begin();
            }
            _udpSocket.send_to(packet.getBufferSequence(), _serverEndpoint, 0, ec);
            if (ec) {
                log::error("Error sending UDP packet: {}", ec.message());
            }
        }
    }

    std::optional<net::NetworkEvent> ClientNetwork::pollEvent(void)
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);
        
        if (!_eventQueue.empty()) {
            net::NetworkEvent event = std::move(_eventQueue.front());
            _eventQueue.pop_front();
            return event;
        }

        return std::nullopt;
    }

    void ClientNetwork::publishEvent(net::NetworkEvent event)
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);
        _eventQueue.push_back(std::move(event));
    }

    std::vector<net::Packet> ClientNetwork::hasPendingPackets(void)
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);
        std::vector<net::Packet> packets;
        for (const auto& event : _eventQueue) {
            packets.push_back(event.packet);
        }
        return packets;
    }

    net::Packet ClientNetwork::popPacket(void)
    {
        std::lock_guard<std::mutex> lock(_eventQueueMutex);
        if (_eventQueue.empty()) {
            throw std::runtime_error("No packets available to pop.");
        }
        net::Packet packet = std::move(_eventQueue.front().packet);
        _eventQueue.pop_front();
        return packet;
    }

    void ClientNetwork::readTcpHeader(void)
    {
        asio::async_read(
            _tcpSocket,
            asio::buffer(&_tcpHeader, sizeof(net::Header)),
            [this](const asio::error_code& error, std::size_t)
            {
                if (error) {
                    log::error("TCP Read Header Error: {}", error.message());
                    stop();
                    return;
                }

                _tcpHeader.magic = net::Packet::from_network(_tcpHeader.magic);
                _tcpHeader.bodySize = net::Packet::from_network(_tcpHeader.bodySize);
                _tcpHeader.sequenceId = net::Packet::from_network(_tcpHeader.sequenceId);
                _tcpHeader.ackId = net::Packet::from_network(_tcpHeader.ackId);

                log::info(
                    "Received TCP Header - OpCode: {}, BodySize: {}",
                    static_cast<uint8_t>(_tcpHeader.opCode),
                    _tcpHeader.bodySize
                );

                if (_tcpHeader.magic != net::MAGIC_NUMBER) {
                    log::error("Invalid Magic Number from Server");
                    readTcpHeader();
                    return;
                }

                if (_tcpHeader.bodySize > 0) {
                    _tcpBody.resize(_tcpHeader.bodySize);
                    readTcpBody();
                } else {
                    net::Packet packet;
                    packet.header = _tcpHeader;

                    if (_tcpHeader.opCode == net::OpCode::Welcome) {
                        log::error("Welcome packet without sessionId body");
                    }

                    publishEvent({0, packet});
                    readTcpHeader();
                }
            }
        );
    }


    void ClientNetwork::readTcpBody(void)
    {
        asio::async_read(
            _tcpSocket,
            asio::buffer(_tcpBody.data(), _tcpBody.size()),
            [this](const asio::error_code& error, std::size_t)
            {
                if (error) {
                    log::error("TCP Read Body Error: {}", error.message());
                    stop();
                    return;
                }

                net::Packet packet;
                packet.header = _tcpHeader;
                packet.body   = _tcpBody;

                if (_tcpHeader.opCode == net::OpCode::Welcome) {
                    uint32_t sessionId = 0;
                    packet >> sessionId;

                    _sessionId = sessionId;

                    log::info("Session ID reçu: {}", _sessionId);

                    this->sendUdpHandshake();
                }

                publishEvent({0, packet});
                readTcpHeader();
            }
        );
    }


    void ClientNetwork::readUdp(void)
    {
        _udpSocket.async_receive_from(
            asio::buffer(_udpBuffer),
            _udpSenderEndpoint,
            [this](const asio::error_code& error, std::size_t bytesReceived)
            {
                if (error) {
                    if (error != asio::error::operation_aborted)
                        log::error("UDP receive error: {}", error.message());
                    return;
                }

                if (bytesReceived < sizeof(net::Header)) {
                    readUdp();
                    return;
                }

                net::Header header;
                std::memcpy(&header, _udpBuffer.data(), sizeof(header));

                header.magic      = net::Packet::from_network(header.magic);
                header.sequenceId = net::Packet::from_network(header.sequenceId);
                header.bodySize   = net::Packet::from_network(header.bodySize);
                header.ackId      = net::Packet::from_network(header.ackId);
                header.sessionId  = net::Packet::from_network(header.sessionId);

                if (header.magic != net::MAGIC_NUMBER) {
                    readUdp();
                    return;
                }

                if (bytesReceived != sizeof(header) + header.bodySize) {
                    log::error("Malformed UDP packet");
                    readUdp();
                    return;
                }

                net::Packet packet;
                packet.header = header;
                packet.body.resize(header.bodySize);

                std::memcpy(
                    packet.body.data(),
                    _udpBuffer.data() + sizeof(header),
                    header.bodySize
                );

                publishEvent({ header.sessionId, packet });
                readUdp();
            }
        );
    }
    
    void ClientNetwork::sendUdpHandshake(void)
    {
        log::info("sendUdpHandshake() called: bound={}, sessionId={}", _udpBound, _sessionId);

        if (_udpBound || _sessionId == 0)
            return;

        net::Packet p(net::OpCode::Hello);
        p.header.sessionId = _sessionId;
        sendPacket(p, net::NetworkMode::UDP);

        _udpBound = true;
        log::info("UDP handshake envoyé (session {})", _sessionId);
    }

    bool ClientNetwork::isUdpReady(void) const
    {
        return _udpBound;
    }


} // namespace rtp::client