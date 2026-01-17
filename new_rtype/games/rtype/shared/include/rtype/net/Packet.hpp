/**
 * File   : Packet.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_SHARED_PACKET_HPP_
    #define RTYPE_SHARED_PACKET_HPP_

    #include "engine/math/Vec2.hpp"
    #include "engine/net/NetworkUtils.hpp"

    #include <bit>
    #include <cstdint>
    #include <cstring>
    #include <stdexcept>
    #include <string>
    #include <string_view>
    #include <type_traits>
    #include <vector>

namespace rtp::net
{
    constexpr std::endian NATIVE_ENDIAN = std::endian::native;  /**< Native endianness of the machine */
    constexpr uint16_t MAGIC_NUMBER = 0xA1B2;                   /**< Magic number for packet validation */
    constexpr uint32_t MAX_STRING_SIZE = 2048;                  /**< Maximum size for strings */
    constexpr uint32_t MAX_VECTOR_SIZE = 8192;                  /**< Maximum size for serialized vectors */
    constexpr uint32_t MAX_BODY_SIZE = 64 * 1024;               /**< Maximum size for packet body */

    /**
     * @enum OpCode
     * @brief Operation codes for different packet types
     */
    enum class OpCode : uint8_t {
        None = 0x00,

        /******************
        * Connection Management
        ******************/
        Hello = 0x01,               /**< Initial handshake packet */
        Welcome = 0x02,             /**< Server welcome response */
        Disconnect = 0x03,          /**< Disconnect notification */

        /******************
        * Authentication
        ******************/
        LoginRequest = 0x1A,        /**< Client login request */
        RegisterRequest = 0x1B,     /**< Client registration request */
        LoginResponse = 0x9A,       /**< Server login response */
        RegisterResponse = 0x9B,    /**< Server registration response */

        /******************
        * Room Management
        ******************/
        ListRooms = 0x04,           /**< Request for room list */
        RoomList = 0x05,            /**< Response with room list */
        CreateRoom = 0x06,          /**< Request to create a room */
        JoinRoom = 0x07,            /**< Request to join a room */
        LeaveRoom = 0x08,           /**< Request to leave the current room */
        RoomUpdate = 0x09,          /**< Notification of room update */
        SetReady = 0x0A,            /**< Set player readiness status */
        RoomChatSended = 0x0B,      /**< Chat message sent in room */
        RoomChatReceived = 0x0C,    /**< Chat message received in room */
        StartGame = 0x0D,           /**< Notification to start the game */

        /******************
        * Game State (Client -> Server)
        ******************/
        InputTick = 0x10,           /**< Client input state */

        /******************
        * Gameplay (Server -> Client)
        ******************/
        EntitySpawn = 0x21,         /**< Entity spawn notification */
        EntityDeath = 0x22,         /**< Entity death notification */
        AmmoUpdate = 0x23,          /**< Ammo update notification */
        Ping = 0x24,                /**< Ping request */
        Pong = 0x25,                /**< Pong response */
        DebugModeUpdate = 0x26,     /**< Debug mode toggle */
        Kicked = 0x27               /**< Player kicked notification */
    };

    #pragma pack(push, 1)

    struct Header {
        uint16_t magic = MAGIC_NUMBER;      /**< Magic number for validation */
        uint16_t sequenceId = 0;            /**< Packet sequence number */
        uint32_t bodySize = 0;              /**< Size of the packet body */
        uint16_t ackId = 0;                 /**< Last acknowledged packet */
        OpCode opCode = OpCode::None;       /**< Operation code */
        uint8_t reserved = 0;               /**< Reserved for future use */
        uint32_t sessionId = 0;             /**< Session identifier */
    };

    using Vec2f = engine::math::Vec2f;      /**< 2D vector with float components */

    struct ConnectPayload {
        uint32_t sessionId;                 /**< Session identifier */
    };

    struct BooleanPayload {
        uint8_t status;                     /**< Status code */
    };

    struct LoginPayload {
        char username[32];                  /**< Player username */
        char password[32];                  /**< Player password */
    };

    struct RegisterPayload {
        char username[32];                  /**< Player username */
        char password[32];                  /**< Player password */
    };

    struct LoginResponsePayload {
        uint8_t success;                    /** Login success flag */
        char username[32];                  /**< Player username */
    };

    struct RegisterResponsePayload {
        uint8_t success;                    /** Registration success flag */
        char username[32];                  /**< Player username */
    };

    struct RoomInfo {
        uint32_t roomId;                    /**< Room identifier */
        char roomName[64];                  /**< Room name */
        uint32_t currentPlayers;            /**< Current number of players */
        uint32_t maxPlayers;                /**< Maximum number of players */
        uint8_t inGame;                     /**< Is the game in progress */
        float difficulty;                   /**< Difficulty level */
        float speed;                        /**< Speed multiplier */
        uint32_t duration;                  /**< Duration of the game session */
        uint32_t seed;                      /**< Seed for random generation */
        uint32_t levelId;                   /**< Level identifier */
    };

    struct CreateRoomPayload {
        char roomName[64];                  /**< Desired room name */
        uint32_t maxPlayers;                /**< Maximum number of players */
        float difficulty;                   /**< Difficulty level */
        float speed;                        /**< Speed multiplier */
        uint32_t levelId;                   /**< Level identifier */
        uint32_t seed;                      /**< Seed for random generation */
        uint32_t duration;                  /**< Duration of the game session */
    };

    struct JoinRoomPayload {
        uint32_t roomId;                    /**< Room identifier to join */
        uint8_t isSpectator;                /**< 1 if joining as spectator */
    };

    struct RoomSnapshotPayload {
        uint32_t roomId;                    /**< Room identifier */
        uint32_t currentPlayers;            /**< Current number of players */
        uint32_t serverTick;                /**< Network entity identifier */
        uint16_t entityCount;               /**< Entity position */
        uint8_t inGame;                     /**< Is the game in progress */
    };

    struct SetReadyPayload {
        uint8_t isReady;                    /**< Player readiness status */
    };

    struct RoomChatPayload {
        char message[256];                  /**< Chat message content */
    };

    struct RoomChatReceivedPayload {
        uint32_t sessionId;                 /**< Sender's session identifier */
        char username[32];                  /**< Sender's username */
        char message[256];                  /**< Chat message content */
    };

    struct EntitySnapshotPayload {
        uint32_t netId;                     /**< Network entity identifier */
        Vec2f position;                     /**< Entity position */
        Vec2f velocity;                     /**< Entity velocity */
        float rotation;                     /**< Entity rotation */
    };

    struct EntitySpawnPayload {
        uint32_t netId;                     /**< Network entity identifier */
        uint8_t type;                       /**< Entity type from EntityType */
        float posX;                         /**< Spawn X position */
        float posY;                         /**< Spawn Y position */
        float sizeX{0.0f};                  /**< Optional width for static entities */
        float sizeY{0.0f};                  /**< Optional height for static entities */
    };

    struct EntityDeathPayload {
        uint32_t netId;                     /**< Network entity identifier */
        uint8_t type;                       /**< Entity type */
        Vec2f position;                     /**< Death position */
    };

    struct AmmoUpdatePayload {
        uint16_t current;                   /**< Current ammo */
        uint16_t max;                       /**< Max ammo */
        uint8_t isReloading;                /**< 1 if reloading */
        float cooldownRemaining;            /**< Remaining reload time */
    };

    struct PingPayload {
        uint64_t clientTimeMs;              /**< Client timestamp in milliseconds */
    };

    struct DebugModePayload {
        uint8_t enabled;                    /**< Debug mode enabled flag */
    };

    struct InputPayload {
        uint8_t inputMask;                  /**< Bitmask of input states */
    };

    #pragma pack(pop)

    class Packet {
        public:
            Header header{};
            std::vector<uint8_t> body{};

            Packet() = default;
            explicit Packet(OpCode op) { header.opCode = op; }

            void resetRead(void) { _readPos = 0; }

            [[nodiscard]]
            engine::net::ByteBuffer serialize(void) const;

            [[nodiscard]]
            static Packet deserialize(engine::net::ByteSpan data);

            template <typename T>
            static inline T to_network(T v);

            template <typename T>
            static inline T from_network(T value);

            template <typename T>
            Packet &operator<<(const T &data);

            template <typename T>
            Packet &operator<<(const std::vector<T> &vec);

            Packet &operator<<(std::string_view str)
            {
                if (str.size() > MAX_STRING_SIZE) {
                    throw std::runtime_error("Packet::operator<< string too large");
                }
                *this << static_cast<uint32_t>(str.size());
                _bumpBodySizeOrThrow(str.size());
                const size_t i = body.size() - str.size();
                std::memcpy(body.data() + i, str.data(), str.size());
                return *this;
            }

            template <typename T>
            Packet &operator>>(T &data);

            template <typename T>
            Packet &operator>>(std::vector<T> &vec);

            Packet &operator>>(std::string &str)
            {
                uint32_t size = 0;
                *this >> size;
                if (size > MAX_STRING_SIZE) {
                    throw std::runtime_error("Packet::operator>> string too large");
                }
                if (_readPos + size > body.size()) {
                    throw std::runtime_error("Packet::operator>> out of bounds");
                }
                str.assign(reinterpret_cast<const char *>(body.data() + _readPos), size);
                _readPos += size;
                return *this;
            }

        private:
            void _bumpBodySizeOrThrow(size_t delta);

            size_t _readPos = 0;
    };
}

#include "Packet.tpp"

#endif /* !RTYPE_SHARED_PACKET_HPP_ */
