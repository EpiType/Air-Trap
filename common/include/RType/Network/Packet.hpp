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

    //////////////////////////////////////////////////////////////////////////
    // Constants and Enums
    //////////////////////////////////////////////////////////////////////////

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
     * @brief Maximum sizes for strings, vectors, and packet bodies
     */
    constexpr uint32_t MAX_STRING_SIZE = 2048;

    /**
     * @brief Maximum size for serialized vectors
     */
    constexpr uint32_t MAX_VECTOR_SIZE = 8192;

    /**
     * @brief Maximum size for packet body
     */
    constexpr uint32_t MAX_BODY_SIZE   = 64 * 1024;

    /**
     * @enum OpCode
     * @brief Operation codes for different packet types
     */
    enum class OpCode : uint8_t {
        None = 0x00,                    /**< No operation */

        // Connection Management
        Hello = 0x01,                   /**< Client hello packet */
        Welcome = 0x02,                 /**< Server welcome response */
        Disconnect = 0x03,              /**< Disconnect notification */

        // Authentication
        LoginRequest = 0x1A,            /**< Client login request */
        RegisterRequest = 0x1B,         /**< Server login response */
        LoginResponse = 0x9A,           /**< Successful connection notification */
        RegisterResponse = 0x9B,        /**< Incorrect password notification */

        // Lobby Management
        ListRooms = 0x04,               /**< Request for room list */
        RoomList = 0x05,                /**< Response with room list */
        CreateRoom = 0x06,              /**< Request to create a room */
        JoinRoom = 0x07,                /**< Request to join a room */
        LeaveRoom = 0x08,               /**< Request to leave a room */
        RoomUpdate = 0x09,              /**< Notification of room update */
        SetReady = 0x0A,                /**< Set player readiness status */
        RoomChatSended = 0x0B,          /**< Chat message in room */
        RoomChatReceived = 0x0C,        /**< Chat message received in room */
        StartGame = 0x0D,               /**< Notification to start the game */

        // Gameplay (C -> S)
        InputTick = 0x10,               /**< Client input state */

        // Game State (S -> C)
        // RoomUpdate = 0x20,             /**< Entity state snapshot */
        EntitySpawn = 0x21,             /**< Entity spawn notification */
        EntityDeath = 0x22,             /**< Entity death notification */
        AmmoUpdate = 0x23,              /**< Ammo update notification */
        Ping = 0x24,                    /**< Ping request */
        Pong = 0x25,                    /**< Ping response */
        DebugModeUpdate = 0x26,         /**< Debug mode toggle */
        Kicked = 0x27                   /**< Player kicked notification */
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
        uint32_t sessionId = 0;         /**< Session identifier */
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
        Bullet = 5,
        PowerupHeal = 6,        // Red - Health regen
        PowerupSpeed = 7,       // (unused for now)
        Obstacle = 8,
        EnemyBullet = 9,
        ObstacleSolid = 10,
        ChargedBullet = 11,
        PowerupDoubleFire = 12, // Yellow/White - Double fire for 20s
        PowerupShield = 13,      // Green - Shield (absorb 1 hit)
        BossShield = 14
    };

    //////////////////////////////////////////////////////////////////////////
    // Payload Structures
    //////////////////////////////////////////////////////////////////////////

    /***** Connection Management *****/
    /**
     * @struct ConnectPayload
     * @brief Player connection data sended
     * @callergraph Client
     * @related Hello OpCode
     */
    struct ConnectPayload {
        uint32_t sessionId;             /**< Session identifier */
    };

    /**
     * @struct BooleanPayload
     * @brief Generic boolean response payload
     */
    struct BooleanPayload {
        uint8_t status;                 /**< Status code */
    };

    /***** Authentication *****/
    /**
     * @struct PlayerLoginPayload
     * @brief Player login data sended
     * @callergraph Client
     * @related LoginRequest OpCode
     */
    struct LoginPayload {
        char username[32];              /**< Player username */
        char password[32];              /**< Player password */
    };

    /**
     * @struct RegisterPayload
     * @brief Player registration data sended by client
     * @callergraph Client
     * @related RegisterRequest OpCode
     */
    struct RegisterPayload {
        char username[32];              /**< Player username */
        char password[32];              /**< Player password */
    };

    /**
     * @struct LoginResponsePayload
     * @brief Login response data sent by server for LoginResponse
     * @callergraph Server
     * @related LoginResponse OpCode
     */
    struct LoginResponsePayload {
        uint8_t success;                /**< Login success flag */
        char username[32];              /**< Player username */
    };

    /**
     * @struct RegisterResponsePayload
     * @brief Registration response data sent by server for RegisterResponse
     * @callergraph Server
     * @related RegisterResponse OpCode
     */
    struct RegisterResponsePayload {
        uint8_t success;                /**< Registration success flag */
        char username[32];              /**< Player username */
    };

    /***** Lobby Management *****/
    /**
     * @struct RoomInfo
     * @brief Information about a game room
     * @callergraph Server
     * @related RoomList OpCode
     */
    struct RoomInfo {
        uint32_t roomId;                /**< Room identifier */
        char roomName[64];              /**< Room name */
        uint32_t currentPlayers;        /**< Current number of players */
        uint32_t maxPlayers;            /**< Maximum number of players */
        uint8_t inGame;                 /**< Is the game in progress */
        float difficulty;               /**< Difficulty level */
        float speed;                    /**< Speed multiplier */
        uint32_t duration;              /**< Duration of the game session */
        uint32_t seed;                  /**< Seed for random generation */
        uint32_t levelId;               /**< Level identifier */
    };

    /**
     * @struct CreateRoomPayload
     * @brief Data for creating a new room
     * @callergraph Client
     * @related CreateRoom OpCode
     */
    struct CreateRoomPayload {
        char roomName[64];              /**< Desired room name */
        uint32_t maxPlayers;            /**< Maximum number of players */
        float difficulty;               /**< Difficulty level */
        float speed;                    /**< Speed multiplier */
        uint32_t levelId;               /**< Level identifier */ // not used
        uint32_t seed;                  /**< Seed for random generation */ // not used
        uint32_t duration;              /**< Duration of the game session */ // not used
    };

    /**
     * @struct JoinRoomPayload
     * @brief Data for joining an existing room
     * @callergraph Client
     * @related JoinRoom OpCode
     */
    struct JoinRoomPayload {
        uint32_t roomId;                /**< Room identifier to join */
        uint8_t isSpectator;            /**< 1 if joining as spectator */
    };

    /**
     * @struct RoomUpdatePayload
     * @brief Data for room update notifications
     * @callergraph Server
     * @related RoomUpdate OpCode
     */
    struct RoomSnapshotPayload {
        uint32_t roomId;                /**< Room identifier */
        uint32_t currentPlayers;        /**< Current number of players */
        uint32_t serverTick;            /**< Network entity identifier */
        uint16_t entityCount;           /**< Entity position */
        uint8_t inGame;                 /**< Is the game in progress */
    };

    /**
     * @struct SetReadyPayload
     * @brief Data for setting player readiness status
     * @callergraph Client
     * @related SetReady OpCode
     */
    struct SetReadyPayload {
        uint8_t isReady;                /**< Player readiness status */
    };

    /**
     * @struct RoomChatPayload
     * @brief Data for sending chat messages in a room
     * @callergraph Client
     * @related RoomChatSended OpCode
     */
    struct RoomChatPayload {
        char message[256];              /**< Chat message content */
    };

    /**
     * @struct RoomChatReceivedPayload
     * @brief Data for receiving chat messages in a room
     * @callergraph Server
     * @related RoomChatReceived OpCode
     */
    struct RoomChatReceivedPayload {
        uint32_t sessionId;             /**< Sender's session identifier */
        char username[32];              /**< Sender's username */
        char message[256];              /**< Chat message content */
    };

    /***** Gameplay *****/
    /**
     * @struct EntitySnapshotPayload
     * @brief Entity state snapshot data
     */
    struct EntitySnapshotPayload {
        uint32_t netId;                 /**< Network entity identifier */
        Vec2f position;                 /**< Entity position */
        Vec2f velocity;                 /**< Entity velocity */
        float rotation;                 /**< Entity rotation */
    };

    /**
     * @struct EntitySpawnPayload
     * @brief Entity spawn notification data
     */
    struct EntitySpawnPayload {
        uint32_t netId;                 /**< Network entity identifier */
        uint8_t type;                   /**< Entity type from EntityType */
        float posX;                     /**< Spawn X position */
        float posY;                     /**< Spawn Y position */
        float sizeX{0.0f};              /**< Optional width for static entities */
        float sizeY{0.0f};              /**< Optional height for static entities */
    };

    /**
     * @struct EntityDeathPayload
     * @brief Entity death notification data
     */
    struct EntityDeathPayload {
        uint32_t netId;                 /**< Network entity identifier */
        uint8_t type;                   /**< Entity type */
        Vec2f position;                 /**< Death position */
    };

    /**
     * @struct AmmoUpdatePayload
     * @brief Ammo update notification data
     * @callergraph Server
     * @related AmmoUpdate OpCode
     */
    struct AmmoUpdatePayload {
        uint16_t current;               /**< Current ammo */
        uint16_t max;                   /**< Max ammo */
        uint8_t isReloading;            /**< 1 if reloading */
        float cooldownRemaining;        /**< Remaining reload time */
    };

    /**
     * @struct PingPayload
     * @brief Ping request/response payload
     * @callergraph Client/Server
     * @related Ping/Pong OpCodes
     */
    struct PingPayload {
        uint64_t clientTimeMs;          /**< Client timestamp in ms */
    };

    /**
     * @struct DebugModePayload
     * @brief Debug mode toggle data
     * @callergraph Server
     * @related DebugModeUpdate OpCode
     */
    struct DebugModePayload {
        uint8_t enabled;                /**< 1 if debug mode enabled */
    };

    #pragma pack(pop)

    //////////////////////////////////////////////////////////////////////
    // Packet Class
    //////////////////////////////////////////////////////////////////////

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
        uint8_t inputMask;              /**< Bitmask of input states */
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
            static inline T to_network(T v)
            {
                if constexpr (std::is_enum_v<T>) {
                    using U = std::underlying_type_t<T>;
                    return static_cast<T>(to_network(static_cast<U>(v)));
                } else if constexpr (std::is_integral_v<T>) {
                    if constexpr (sizeof(T) > 1 && NATIVE_ENDIAN == std::endian::little)
                        return std::byteswap(v);
                    return v;
                } else if constexpr (std::is_same_v<T, float>) {
                    if constexpr (NATIVE_ENDIAN == std::endian::little) {
                        uint32_t u = std::bit_cast<uint32_t>(v);
                        u = std::byteswap(u);
                        return std::bit_cast<float>(u);
                    }
                    return v;
                }
                return v;
            }

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
            void _bumpBodySizeOrThrow(); /**< Increment body size and check for overflow */
            
            size_t _readPos = 0;         /**< Current read position in body */

            mutable Header _cacheHeader; /**< Cached header with network endianness */
    };
}   

#include "Packet.tpp"

#endif /* !RTYPE_NETWORK_PACKET_HPP_ */
