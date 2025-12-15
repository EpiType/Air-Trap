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
          _serverEndpoint(asio::ip::make_address(serverIp), serverPort),
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
                _ioContext.run();
            });
            asio::error_code ec;
            _tcpSocket.connect(asio::ip::tcp::endpoint(
                asio::ip::make_address(_serverIp, ec),
                _serverPort
            ), ec);
            log::info("Connected to server {}:{}", _serverIp, _serverPort);
        }
        catch (const asio::system_error& e) {
            log::error("Failed to start Client Network: {}", e.what());
            return this->stop();
        }
        catch (const std::exception& e) {
            log::fatal("Unexpected error in Client Network start: {}", e.what());
            return this->stop();
        }
    }

    void ClientNetwork::stop(void)
    {
        _ioContext.stop();
        if (_ioThread.joinable()) {
            _ioThread.join();
            log::info("ASIO I/O thread stopped.");
        }

        _tcpSocket.close();
        _udpSocket.close();
    };

    void ClientNetwork::sendPacket(const net::Packet &packet, net::NetworkMode mode)
    {
        asio::error_code ec;
        if (mode == net::NetworkMode::TCP) {
            asio::write(_tcpSocket, asio::buffer(packet.getBufferSequence()), ec);
            if (ec) {
                log::error("Error sending TCP packet: {}", ec.message());
            }
        } else if (mode == net::NetworkMode::UDP) {
            _udpSocket.send_to(asio::buffer(packet.getBufferSequence()), _serverEndpoint, 0, ec);
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

} // namespace rtp::client