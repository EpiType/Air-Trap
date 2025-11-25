#pragma once

#include "network/UdpSocket.hpp"
#include "network/Protocol.hpp"
#include <memory>
#include <thread>
#include <atomic>
#include <string>

namespace network {

/**
 * @brief Client-side network system.
 * 
 * Sends player inputs to server and receives game state updates.
 */
class NetworkClient {
public:
    NetworkClient(const std::string& serverIp, std::uint16_t serverPort);
    ~NetworkClient();

    // Connect to server
    bool connect();

    // Disconnect from server
    void disconnect();

    // Send packet to server
    void sendToServer(const std::vector<std::uint8_t>& data);

    // Set callback for received packets
    using PacketCallback = std::function<void(const PacketHeader&, const std::vector<std::uint8_t>&)>;
    void setPacketCallback(PacketCallback callback);

private:
    void networkThread();

    std::string _serverIp;
    std::uint16_t _serverPort;
    std::unique_ptr<UdpSocket> _socket;
    std::thread _networkThread;
    std::atomic<bool> _running{false};
    PacketCallback _packetCallback;
};

} // namespace network
