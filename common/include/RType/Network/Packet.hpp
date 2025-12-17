/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Packet.hpp, Packet
 * declaration
*/

/**
 * @file Packet.hpp
 * @brief Network packet implementation for R-Type protocol
 */

#ifndef RTYPE_NETWORK_PACKET_HPP_
    #define RTYPE_NETWORK_PACKET_HPP_

    #include "RType/ECS/ComponentConcept.hpp"
    #include "RType/Math/Vec2.hpp"

    #include <bit>
    #include <cstdint>
    #include <cstring>
    #include <span>
    #include <stdexcept>
    #include <string>
    #include <string_view>
    #include <utility>
    #include <vector>
    #include <asio/buffer.hpp>
    #include <array>
    #include <type_traits>

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type protocol
 */
namespace rtp::net
{
    /**
     * @brief Native endianness of the machine
     */
    constexpr std::endian NATIVE_ENDIAN = std::endian::native;
    
    /**
     * @brief Magic number for packet validation
     */
    constexpr uint16_t MAGIC_NUMBER = 0xA1B2;
    
    /**
     * @brief Safe UDP payload size to avoid fragmentation
     */
    constexpr size_t MTU_SIZE = 1400;

    /**
     * @enum OpCode
     * @brief Operation codes for different packet types
     */
    enum class OpCode : uint8_t {
        None = 0x00,           /**< No operation */

        // Connection Management
        Hello = 0x01,          /**< Client hello packet */
        Welcome = 0x02,        /**< Server welcome response */
        Disconnect = 0x03,     /**< Disconnect notification */

        // Lobby Management
        ListRooms = 0x04,     /**< Request for room list */
        RoomList = 0x05,      /**< Response with room list */
        CreateRoom = 0x06,    /**< Request to create a room */
        JoinRoom = 0x07,      /**< Request to join a room */
        LeaveRoom = 0x08,     /**< Request to leave a room */
        RoomUpdate = 0x09,   /**< Notification of room update */
        SetReady = 0x0A,      /**< Set player readiness status */

        // Gameplay (C -> S)
        InputTick = 0x10,      /**< Client input state */

        // Game State (S -> C)
        WorldUpdate = 0x20, /**< Entity state snapshot */
        EntitySpawn = 0x21,    /**< Entity spawn notification */
        EntityDeath = 0x22,     /**< Entity death notification */

        // Game Control
        StartGame = 0x30      /**< Notification to start the game */
    };

    #pragma pack(push, 1)

    /**
     * @struct Header
     * @brief Packet header with sequencing and acknowledgment support
     */
    struct Header {
        uint16_t magic = MAGIC_NUMBER;  /**< Magic number for validation */
        uint16_t sequenceId = 0;        /**< Packet sequence number */
        uint32_t bodySize = 0;          /**< Size of the packet body */
        uint16_t ackId = 0;             /**< Last acknowledged packet */
        OpCode opCode = OpCode::None;   /**< Operation code */
        uint8_t reserved = 0;           /**< Reserved for future use */
        uint32_t sessionId;             /**< Session identifier */
    };

    /**
     * @enum EntityType
     * @brief Types of entities in the game
     */
    enum class EntityType : uint8_t {
        Player = 1,
        Scout  = 2,
        Tank   = 3,
        Boss   = 4,
        Bullet = 5
    };

    /**
     * @struct EntitySnapshotPayload
     * @brief Entity state snapshot data
     */
    struct EntitySnapshotPayload {
        uint32_t netId;         /**< Network entity identifier */
        Vec2f position;         /**< Entity position */
        Vec2f velocity;         /**< Entity velocity */
        float rotation;         /**< Entity rotation */
    };

    /**
     * @struct WorldSnapshotPayload
     * @brief World state snapshot data
     */
    struct WorldSnapshotPayload {
        uint32_t serverTick;    /**< Network entity identifier */
        uint16_t entityCount;   /**< Entity position */
    };

    /**
     * @struct EntitySpawnPayload
     * @brief Entity spawn notification data
     */
    struct EntitySpawnPayload {
        uint32_t netId;         /**< Network entity identifier */
        uint8_t type;           /**< Entity type */
        Vec2f position;         /**< Spawn position */
        EntityType entityType;  /**< Type of the entity */
    };

    /**
     * @struct EntityDeathPayload
     * @brief Entity death notification data
     */
    struct EntityDeathPayload {
        uint32_t netId;         /**< Network entity identifier */
        uint8_t type;           /**< Entity type */
        Vec2f position;         /**< Death position */
    };

    #pragma pack(pop)

    /**
     * @using BufferSequence
     * @brief Buffer sequence for efficient multi-part network transmission
     * 
     * Represents a sequence of constant buffers (header and body) that can be
     * sent atomically to avoid multiple system calls. Allows ASIO to gather
     * and send packet header and body in a single operation.
     */
    using BufferSequence = std::array<asio::const_buffer, 2>;

    /**
     * @struct InputPayload
     * @brief Client input state data
     */
    struct InputPayload {
        uint8_t inputMask;      /**< Bitmask of input states */
    };

    /**
     * @class Packet
     * @brief Network packet with header and serializable body
     * 
     * Provides serialization and deserialization operators for various data types.
     * Supports automatic endianness handling and buffer management.
     */
    class Packet {
        public:
            Header header;              /**< Packet header */
            std::vector<uint8_t> body;  /**< Packet body/payload */

            /**
             * @brief Default constructor
             */
            Packet() = default;
            
            /**
             * @brief Construct packet with specific operation code
             * @param op Operation code for the packet
             */
            explicit Packet(OpCode op);

            /**
             * @brief Reset read position to beginning of body
             */
            void resetRead(void);
            
            /**
             * @brief Get buffer sequence for network transmission
             * @return Buffer sequence containing header and body
             */
            BufferSequence getBufferSequence(void) const;

            /**
             * @brief Converts a primitive type (integer, float) from machine endianness to Big-Endian (network).
             * @note Uses std::byteswap for endianness conversion if necessary.
             * @param value The value to convert.
             * @return The value in network endianness format.
             */
            template <typename T>
            static inline T to_network(T value) {
                if constexpr (std::is_integral_v<T>) {
                    if constexpr (sizeof(T) > 1 && NATIVE_ENDIAN == std::endian::little) {
                        return std::byteswap(value); 
                    }
                }
                return value;
            };

            /**
             * @brief Converts a primitive type (integer, float) from Big-Endian (network) to machine endianness.
             * @note Uses std::byteswap for endianness conversion if necessary.
             * @param value The value to convert.
             * @return The value in machine endianness format.
             */
            template <typename T>
            static inline T from_network(T value) {
                return to_network(value);
            };

            /**
             * @brief Serialize data into packet body
             * @tparam T Serializable type
             * @param data Data to serialize
             * @return Reference to this packet for chaining
             */
            template <typename T>
            auto operator<<(T data) -> Packet &;

            /**
             * @brief Serialize vector into packet body
             * @tparam T Serializable type
             * @param vec Vector to serialize
             * @return Reference to this packet for chaining
             */
            template <typename T>
            auto operator<<(const std::vector<T> &vec) -> Packet &;
            
            /**
             * @brief Serialize string into packet body
             * @param str String view to serialize
             * @return Reference to this packet for chaining
             */
            auto operator<<(std::string_view str) -> Packet &;

            /**
             * @brief Deserialize data from packet body
             * @tparam T Serializable type
             * @param data Reference to store deserialized data
             * @return Reference to this packet for chaining
             */
            template <typename T>
            auto operator>>(T &data) -> Packet &;

            /**
             * @brief Deserialize vector from packet body
             * @tparam T Serializable type
             * @param vec Reference to store deserialized vector
             * @return Reference to this packet for chaining
             */
            template <typename T>
            auto operator>>(std::vector<T> &vec) -> Packet &;

            /**
             * @brief Deserialize string from packet body
             * @param str Reference to store deserialized string
             * @return Reference to this packet for chaining
             */
            auto operator>>(std::string &str) -> Packet &;

        private:
            size_t _readPos = 0;         /**< Current read position in body */

            mutable Header _cacheHeader; /**< Cached header with network endianness */
    };
}   

#include "Packet.tpp"

#endif /* !RTYPE_NETWORK_PACKET_HPP_ */
