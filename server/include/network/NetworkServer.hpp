#pragma once

#include "network/UdpSocket.hpp"
#include "network/Protocol.hpp"
#include <memory>
#include <thread>
#include <atomic>

namespace network {

/**
 * @brief Server-side network system.
 * 
 * Multi-threaded to handle client messages without blocking game logic.
 */
class NetworkServer {
public:
    NetworkServer(std::uint16_t port);
    ~NetworkServer();

    // Start the network thread
    void start();

    // Stop the network thread
    void stop();

    // Send packet to a client
    void sendToClient(const std::string& clientIp, std::uint16_t clientPort, const std::vector<std::uint8_t>& data);

    // Broadcast packet to all connected clients
    void broadcast(const std::vector<std::uint8_t>& data);

    // Set callback for received packets
    using PacketCallback = std::function<void(const PacketHeader&, const std::vector<std::uint8_t>&, const std::string&, std::uint16_t)>;
    void setPacketCallback(PacketCallback callback);

private:
    void networkThread();

    std::uint16_t _port;
    std::unique_ptr<UdpSocket> _socket;
    std::thread _networkThread;
    std::atomic<bool> _running{false};
    PacketCallback _packetCallback;
};

} // namespace network
