/**
 * File   : INetwork.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "RType/Network/Session.hpp"
#include "RType/Logger.hpp"

namespace rtp::net
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    Session::Session(uint32_t id, 
                     asio::ip::tcp::socket socket, 
                     asio::ip::udp::socket& serverUdpSocket,
                     IEventPublisher &publisher)
        : _id(id),
          _socket(std::move(socket)),
          _serverUdpSocket(serverUdpSocket),
          _publisher(publisher),
          _timer(_socket.get_executor())
    {
    }

    Session::~Session() {
        stop();
    }

    void Session::start() {
        asio::co_spawn(_socket.get_executor(), 
            [self = shared_from_this()] { return self->reader(); }, 
            asio::detached);

        asio::co_spawn(_socket.get_executor(), 
            [self = shared_from_this()] { return self->writer(); }, 
            asio::detached);
    }

    void Session::stop() {
        if (!_stopped) {
            _stopped = true;
            _socket.close();
            _timer.cancel();
        }
    }

    void Session::send(const Packet& packet, NetworkMode mode) {
        if (mode == NetworkMode::TCP) {
            std::lock_guard<std::mutex> lock(_writeMutex);
            _writeQueue.push_back(packet);
            _timer.cancel();
        } 
        else if (mode == NetworkMode::UDP && _hasUdp) {
            auto pkt = std::make_shared<Packet>(packet); 
            auto buffers = pkt->getBufferSequence();
            _serverUdpSocket.async_send_to(buffers, _udpEndpoint, 
                [](const asio::error_code&, std::size_t){});
        }
    }

    void Session::setId(uint32_t id) {
        _id = id;
    }

    uint32_t Session::getId() const {
        return _id;
    }

    asio::ip::tcp::endpoint Session::getTcpEndpoint() const {
        return _socket.remote_endpoint();
    }

    void Session::setUdpEndpoint(const asio::ip::udp::endpoint& endpoint) {
        _udpEndpoint = endpoint;
        _hasUdp = true;
    }

    //////////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////////

    asio::awaitable<void> Session::reader() {
        try {
            while (!_stopped) {
                Header header;
                co_await asio::async_read(_socket, asio::buffer(&header, sizeof(Header)), asio::use_awaitable);

                header.magic = Packet::from_network(header.magic);
                header.sequenceId = Packet::from_network(header.sequenceId);
                header.bodySize = Packet::from_network(header.bodySize);
                header.ackId = Packet::from_network(header.ackId);
                header.sessionId = Packet::from_network(header.sessionId);
                
                if (header.magic != MAGIC_NUMBER) break;

                Packet packet;
                packet.header = header;

                if (header.bodySize > 0) {
                    packet.body.resize(header.bodySize);
                    co_await asio::async_read(_socket, asio::buffer(packet.body), asio::use_awaitable);
                }

                _publisher.publishEvent({_id, packet});
            }
        } catch (std::exception&) {
            stop();
            _publisher.publishEvent({_id, Packet(OpCode::Disconnect)});
        }
    }

    asio::awaitable<void> Session::writer() {
        try {
            while (!_stopped) {
                if (_writeQueue.empty()) {
                    asio::error_code ec;
                    _timer.expires_at(std::chrono::steady_clock::time_point::max());
                    co_await _timer.async_wait(redirect_error(asio::use_awaitable, ec));
                } else {
                    Packet packet;
                    {
                        std::lock_guard<std::mutex> lock(_writeMutex);
                        packet = _writeQueue.front();
                        _writeQueue.pop_front();
                    }
                    co_await asio::async_write(_socket, packet.getBufferSequence(), asio::use_awaitable);
                }
            }
        } catch (std::exception&) {
            stop();
        }
    }

} // namespace rtp::net