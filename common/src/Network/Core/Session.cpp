/**
 * File   : Session.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "RType/Network/Core/Session.hpp"
#include "RType/Logger.hpp"
#include "RType/Network/Packet.hpp"
#include "RType/Network/INetwork.hpp"
#include "RType/Network/IEventPublisher.hpp"

namespace rtp::net
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    Session::Session(asio::ip::tcp::socket socket,
                     IEventPublisher &publisher)
        : _socket(std::move(socket)), _publisher(publisher), _timer(_socket.get_executor())
    {
        log::info("Session created with remote endpoint: {}",
                  this->_socket.remote_endpoint().address().to_string());
    }

    Session::~Session()
    {
        log::info("Session with remote endpoint {} destroyed",
                  this->_socket.remote_endpoint().address().to_string());
    }

    void Session::start(void)
    {
        asio::co_spawn(_socket.get_executor(),
                       [self = shared_from_this()]() { return self->reader(); },
                       asio::detached);
        asio::co_spawn(_socket.get_executor(),
                       [self = shared_from_this()]() { return self->writer(); },
                       asio::detached);
    }

    void Session::stop(void)
    {
        _stopped = true;
        asio::error_code ec;
        _socket.close(ec);
        if (ec) {
            log::error("Error closing socket: {}", ec.message());
        } else {
            log::info("Socket closed successfully");
        }
    }

    bool Session::isConnected(void) const
    {
        return _socket.is_open() && !_stopped;
    }

    void Session::sendTcp(const Packet &packet)
    {
        std::lock_guard<std::mutex> lock(_writeMutex);
        _writeQueue.push_back(packet);
    }

    void Session::setUdp(const asio::ip::udp::endpoint& endpoint)
    {
        _udpEndpoint = endpoint;
    }

    asio::ip::udp::endpoint Session::getUdpEndpoint() const
    {
        return _udpEndpoint;
    }

    bool Session::hasUdp() const
    {
        return _udpEndpoint.address().is_v4() || _udpEndpoint.address().is_v6();
    }

    void Session::setUsername(const std::string &username)
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        _username = username;
    }

    std::string Session::getUsername() const
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        return _username;
    }

    void Session::setRoomId(uint32_t roomId)
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        _roomId = roomId;
    }

    uint32_t Session::getRoomId() const
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        return _roomId;
    }

    bool Session::isInRoom() const
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        return _roomId != 0;
    }

    void Session::setState(PlayerState state)
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        _state = state;
    }

    PlayerState Session::getState() const
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        return _state;
    }

    void Session::setReady(bool ready)
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        _isReady = ready;
    }

    bool Session::isReady() const
    {
        std::lock_guard<std::mutex> lock(_stateMutex);
        return _isReady;
    }

    void Session::setId(uint32_t id)
    {
        _id = id;
    }

    uint32_t Session::getId() const
    {
        return _id;
    }

    void Session::resetTimer(void)
    {
        _timer.expires_after(std::chrono::seconds(30));
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private Methods
    ///////////////////////////////////////////////////////////////////////////

    asio::awaitable<void> Session::writer()
    {
        auto self = shared_from_this();

        try {
            for (;;)
            {
                rtp::net::Packet packet;

                /* Wait that a packet arrive */
                {
                    std::unique_lock<std::mutex> lock(_writeMutex);
                    if (_writeQueue.empty()) {
                        lock.unlock();
                        self->_timer.expires_after(std::chrono::milliseconds(1));
                        co_await self->_timer.async_wait(asio::use_awaitable);
                        continue;
                    }

                    packet = std::move(self->_writeQueue.front());
                    self->_writeQueue.pop_front();
                }

                auto bufferSequence = packet.getBufferSequence();
                co_await asio::async_write(self->_socket, bufferSequence, asio::use_awaitable);

            }
        } catch (asio::system_error &e)
        {
            if (e.code() == asio::error::eof) {
                log::info("Connection from {} closed by peer", self->_id);
            } else {
                log::error("Writer of {} system error: {}", self->_id, e.what());
            }
        } catch (std::exception &e)
        {
            log::error("Writer from client {} exception: {}", _id ,e.what());
        }
        self->stop();
    }

    asio::awaitable<void> Session::reader()
    {
        auto self = shared_from_this();

        try {
            rtp::net::Header header;

            for (;;)
            {
                co_await asio::async_read(self->_socket,
                                           asio::buffer(&header, sizeof(rtp::net::Header)),
                                           asio::use_awaitable);

                if (header.magic != rtp::net::MAGIC_NUMBER) {
                    log::warning("Invalid magic number from client {}", self->_id);
                    continue;
                }

                rtp::net::Packet recivedPacket;
                recivedPacket.header = header;

                recivedPacket.body.resize(header.bodySize);
                co_await asio::async_read(self->_socket,
                                               asio::buffer(recivedPacket.body.data(), header.bodySize),
                                               asio::use_awaitable);

                rtp::net::NetworkEvent event;
                event.sessionId = self->_id;
                event.packet = std::move(recivedPacket);
                self->_publisher.publishEvent(std::move(event));

                self->resetTimer();
            }
        } catch (asio::system_error &e)
        {
            if (e.code() == asio::error::eof) {
                log::info("Connection from {} closed by peer", self->_id);
            } else {
                log::error("Writer by {} system error: {}", self->_id, e.what());
            }
        } catch (std::exception &e)
        {
            log::error("Writer exception: {}", e.what());
        }

        self->stop();
    }
} // namespace rtp::net