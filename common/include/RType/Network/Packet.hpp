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
    #include "RType/Math/Vec2f.hpp"

    #include <bit>
    #include <cstdint>
    #include <cstring>
    #include <span>
    #include <stdexcept>
    #include <string>
    #include <string_view>
    #include <utility>
    #include <vector>

/**
 * @namespace rtp::net
 * @brief Network layer for R-Type protocol
 */
namespace rtp::net
{
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

        // Gameplay (C -> S)
        InputTick = 0x10,      /**< Client input state */

        // Game State (S -> C)
        EntitySnapshot = 0x20, /**< Entity state snapshot */
        EntitySpawn = 0x21,    /**< Entity spawn notification */
        EntityDeath = 0x22     /**< Entity death notification */
    };

    #pragma pack(push, 1)

    /**
     * @struct Header
     * @brief Packet header with sequencing and acknowledgment support
     */
    struct Header {
        uint16_t magic = MAGIC_NUMBER;  /**< Magic number for validation */
        uint16_t sequenceId = 0;        /**< Packet sequence number */
        uint16_t ackId = 0;             /**< Last acknowledged packet */
        OpCode opCode = OpCode::None;   /**< Operation code */
        uint8_t reserved = 0;           /**< Reserved for future use */
    };

    /**
     * @struct EntitySnapshotPayload
     * @brief Entity state snapshot data
     */
    struct EntitySnapshotPayload {
        uint32_t netId;         /**< Network entity identifier */
        Vec2f position;         /**< Entity position */
        int16_t angle;          /**< Entity rotation angle */
        uint8_t hp;             /**< Entity health points */
    };

    /**
     * @struct EntitySpawnPayload
     * @brief Entity spawn notification data
     */
    struct EntitySpawnPayload {
        uint32_t netId;         /**< Network entity identifier */
        uint8_t type;           /**< Entity type */
        Vec2f position;         /**< Spawn position */
    };

    /**
     * @struct InputPayload
     * @brief Client input state data
     */
    struct InputPayload {
        uint8_t inputMask;      /**< Bitmask of input states */
    };

    #pragma pack(pop)

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
            auto getBufferSequence(void) const;

            /**
             * @brief Serialize data into packet body
             * @tparam T Serializable type
             * @param data Data to serialize
             * @return Reference to this packet for chaining
             */
            template <rtp::ecs::Serializable T>
            auto operator<<(T data) -> Packet &;

            /**
             * @brief Serialize vector into packet body
             * @tparam T Serializable type
             * @param vec Vector to serialize
             * @return Reference to this packet for chaining
             */
            template <rtp::ecs::Serializable T>
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
            template <rtp::ecs::Serializable T>
            auto operator>>(T &data) -> Packet &;

            /**
             * @brief Deserialize vector from packet body
             * @tparam T Serializable type
             * @param vec Reference to store deserialized vector
             * @return Reference to this packet for chaining
             */
            template <rtp::ecs::Serializable T>
            auto operator>>(std::vector<T> &vec) -> Packet &;

            /**
             * @brief Deserialize string from packet body
             * @param str Reference to store deserialized string
             * @return Reference to this packet for chaining
             */
            auto operator>>(std::string &str) -> Packet &;

        private:
            size_t _readPos = 0; /**< Current read position in body */
    };
}

#endif /* !RTYPE_NETWORK_PACKET_HPP_ */
