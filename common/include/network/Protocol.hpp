#pragma once

#include <cstdint>
#include <vector>

namespace network {

/**
 * @brief Binary network packet structure.
 * 
 * All game communication uses binary UDP packets for performance.
 */

enum class PacketType : std::uint8_t {
    // Client -> Server
    PLAYER_INPUT = 0x01,
    PING = 0x02,
    
    // Server -> Client
    GAME_STATE = 0x10,
    ENTITY_UPDATE = 0x11,
    ENTITY_SPAWN = 0x12,
    ENTITY_DESTROY = 0x13,
    PONG = 0x14,
    
    // Bidirectional
    DISCONNECT = 0xFF
};

#pragma pack(push, 1)

struct PacketHeader {
    PacketType type;
    std::uint32_t sequence;
    std::uint16_t payloadSize;
};

struct PlayerInputPacket {
    PacketHeader header;
    std::uint32_t playerId;
    std::uint32_t inputFlags;
    float deltaTime;
};

struct EntityUpdatePacket {
    PacketHeader header;
    std::uint32_t entityId;
    float posX;
    float posY;
    float velX;
    float velY;
};

#pragma pack(pop)

/**
 * @brief Serializes and deserializes network packets.
 */
class PacketSerializer {
public:
    static std::vector<std::uint8_t> serialize(const PacketHeader& header, const void* payload);
    static bool deserialize(const std::vector<std::uint8_t>& data, PacketHeader& header, void* payload);
};

} // namespace network
