#pragma once

#include <memory>
#include <functional>
#include <vector>
#include <cstdint>

namespace network {

/**
 * @brief UDP socket wrapper using ASIO.
 * 
 * This is a simplified interface. Full implementation would use asio::io_context.
 */
class UdpSocket {
public:
    UdpSocket() = default;
    virtual ~UdpSocket() = default;

    // Bind to a port (server)
    virtual bool bind(std::uint16_t port) = 0;

    // Send data to an endpoint
    virtual bool sendTo(const std::vector<std::uint8_t>& data, const std::string& ip, std::uint16_t port) = 0;

    // Receive data (async callback)
    using ReceiveCallback = std::function<void(const std::vector<std::uint8_t>&, const std::string&, std::uint16_t)>;
    virtual void receiveFrom(ReceiveCallback callback) = 0;

    // Close the socket
    virtual void close() = 0;
};

/**
 * @brief Factory to create platform-specific UDP sockets.
 */
class SocketFactory {
public:
    static std::unique_ptr<UdpSocket> createUdpSocket();
};

} // namespace network
