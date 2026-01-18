/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Packet.tpp
*/

#pragma once
#include <cstring>
#include <stdexcept>
#include <vector>
#include <string>
#include <type_traits>

namespace rtp::net
{
    ///////////////////////////////////////////////////////////////////////////
    // Simple Template Implementations
    ///////////////////////////////////////////////////////////////////////////

    template <typename T>
    auto Packet::operator<<(T data) -> Packet &
    {
        T network_data = to_network(data);

        size_t dataSize = sizeof(T);
        size_t currentSize = body.size();
        body.resize(currentSize + dataSize);

        std::memcpy(body.data() + currentSize, &network_data, dataSize);

        header.bodySize = static_cast<uint32_t>(body.size());

        return *this;
    }

    template <typename T>
    auto Packet::operator>>(T &data) -> Packet &
    {
        if (_readPos + sizeof(T) > body.size()) {
            throw std::out_of_range("Packet read overflow");
        }

        T network_data{};
        std::memcpy(&network_data, body.data() + _readPos, sizeof(T));
        _readPos += sizeof(T);

        data = from_network(network_data);

        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // Specialized Template Implementations
    //////////////////////////////////////////////////////////////////////////

    template <typename T>
    inline auto Packet::operator<<(const std::vector<T> &vec) -> Packet &
    {
        uint32_t vecSize = static_cast<uint32_t>(vec.size());
        *this << vecSize;
        for (const auto &item : vec) {
            *this << item;
        }
        return *this;
    }

    template <typename T>
    inline auto Packet::operator>>(std::vector<T> &vec) -> Packet &
    {
        uint32_t vecSize;
        *this >> vecSize;

        vec.clear();
        if (vecSize > 4096) throw std::runtime_error("Vector too large");
        vec.reserve(vecSize);

        for (uint32_t i = 0; i < vecSize; ++i) {
            T item{};
            *this >> item;
            vec.push_back(std::move(item));
        }
        return *this;
    }

    inline auto Packet::operator<<(std::string_view str) -> Packet &
    {
        uint32_t strSize = static_cast<uint32_t>(str.size());

        *this << strSize;

        size_t currentSize = body.size();
        body.resize(currentSize + strSize);

        std::memcpy(body.data() + currentSize, str.data(), strSize);

        header.bodySize = static_cast<uint32_t>(body.size());

        return *this;
    }

    inline auto Packet::operator>>(std::string &str) -> Packet &
    {
        uint32_t strSize;
        *this >> strSize;

        if (strSize > 4096) throw std::runtime_error("String too large");

        if (_readPos + strSize > body.size()) {
            throw std::out_of_range("Packet read overflow");
        }

        str.assign(reinterpret_cast<const char *>(body.data() + _readPos), strSize);
        _readPos += strSize;
        return *this;
    }

    //////////////////////////////////////////////////////////////////////////
    // Batching Operations
    //////////////////////////////////////////////////////////////////////////

    template <>
    inline auto Packet::operator<<(ConnectPayload data) -> Packet &
    {
        *this << data.sessionId;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(ConnectPayload &data) -> Packet &
    {
        *this >> data.sessionId;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(LoginPayload data) -> Packet &
    {
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        *this << std::string_view(data.password, strnlen(data.password, sizeof(data.password)));
        *this << data.weaponKind;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(LoginPayload &data) -> Packet &
    {
        std::string username;
        std::string password;
        *this >> username;
        *this >> password;
        std::strncpy(data.username, username.c_str(), sizeof(data.username));
        data.username[sizeof(data.username) - 1] = '\0';
        std::strncpy(data.password, password.c_str(), sizeof(data.password));
        data.password[sizeof(data.password) - 1] = '\0';
        *this >> data.weaponKind;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(RegisterPayload data) -> Packet &
    {
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        *this << std::string_view(data.password, strnlen(data.password, sizeof(data.password)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(RegisterPayload &data) -> Packet &
    {
        std::string username;
        std::string password;
        *this >> username;
        *this >> password;
        std::strncpy(data.username, username.c_str(), sizeof(data.username));
        data.username[sizeof(data.username) - 1] = '\0';
        std::strncpy(data.password, password.c_str(), sizeof(data.password));
        data.password[sizeof(data.password) - 1] = '\0';
        return *this;
    }

    template <>
    inline auto Packet::operator<<(LoginResponsePayload data) -> Packet &
    {
        *this << data.success;
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(LoginResponsePayload &data) -> Packet &
    {
        *this >> data.success;
        std::string username;
        *this >> username;
        std::strncpy(data.username, username.c_str(), sizeof(data.username));
        data.username[sizeof(data.username) - 1] = '\0';
        return *this;
    }

    template <>
    inline auto Packet::operator<<(RegisterResponsePayload data) -> Packet &
    {
        *this << data.success;
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(RegisterResponsePayload &data) -> Packet &
    {
        *this >> data.success;
        std::string username;
        *this >> username;
        std::strncpy(data.username, username.c_str(), sizeof(data.username));
        data.username[sizeof(data.username) - 1] = '\0';
        return *this;
    }

    template <>
    inline auto Packet::operator<<(RoomInfo data) -> Packet &
    {
        *this << data.roomId;
        *this << std::string_view(data.roomName, strnlen(data.roomName, sizeof(data.roomName)));
        *this << data.currentPlayers;
        *this << data.maxPlayers;
        *this << data.inGame;
        *this << data.difficulty;
        *this << data.speed;
        *this << data.duration;
        *this << data.seed;
        *this << data.levelId;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(RoomInfo &data) -> Packet &
    {
        *this >> data.roomId;

        std::string roomName;
        *this >> roomName;
        std::strncpy(data.roomName, roomName.c_str(), sizeof(data.roomName));
        data.roomName[sizeof(data.roomName) - 1] = '\0';

        *this >> data.currentPlayers;
        *this >> data.maxPlayers;
        *this >> data.inGame;
        *this >> data.difficulty;
        *this >> data.speed;
        *this >> data.duration;
        *this >> data.seed;
        *this >> data.levelId;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(CreateRoomPayload data) -> Packet &
    {
        *this << std::string_view(data.roomName, strnlen(data.roomName, sizeof(data.roomName)));
        *this << data.maxPlayers;
        *this << data.difficulty;
        *this << data.speed;
        *this << data.levelId;
        *this << data.seed;
        *this << data.duration;
        *this << data.roomType;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(CreateRoomPayload &data) -> Packet &
    {
        std::string roomName;
        *this >> roomName;
        std::strncpy(data.roomName, roomName.c_str(), sizeof(data.roomName));
        data.roomName[sizeof(data.roomName) - 1] = '\0';

        *this >> data.maxPlayers;
        *this >> data.difficulty;
        *this >> data.speed;
        *this >> data.levelId;
        *this >> data.seed;
        *this >> data.duration;
        *this >> data.roomType;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(JoinRoomPayload data) -> Packet &
    {
        *this << data.roomId;
        *this << data.isSpectator;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(JoinRoomPayload &data) -> Packet &
    {
        *this >> data.roomId;
        *this >> data.isSpectator;
        return *this;
    }

    // template <>
    // inline auto Packet::operator<<(LeaveRoomPayload data) -> Packet &
    // {
    //     *this << data.roomId;
    //     return *this;
    // }

    // template <>
    // inline auto Packet::operator>>(LeaveRoomPayload &data) -> Packet &
    // {
    //     *this >> data.roomId;
    //     return *this;
    // }

    template <>
    inline auto Packet::operator<<(RoomSnapshotPayload data) -> Packet &
    {
        *this << data.roomId;
        *this << data.currentPlayers;
        *this << data.serverTick;
        *this << data.entityCount;
        *this << data.inGame;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(RoomSnapshotPayload &data) -> Packet &
    {
        *this >> data.roomId;
        *this >> data.currentPlayers;
        *this >> data.serverTick;
        *this >> data.entityCount;
        *this >> data.inGame;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(SetReadyPayload data) -> Packet &
    {
        *this << data.isReady;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(SetReadyPayload &data) -> Packet &
    {
        *this >> data.isReady;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(RoomChatPayload data) -> Packet &
    {
        *this << std::string_view(data.message, strnlen(data.message, sizeof(data.message)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(RoomChatPayload &data) -> Packet &
    {
        std::string msg;
        *this >> msg;
        std::strncpy(data.message, msg.c_str(), sizeof(data.message));
        data.message[sizeof(data.message) - 1] = '\0';
        return *this;
    }

    template <>
    inline auto Packet::operator<<(RoomChatReceivedPayload data) -> Packet &
    {
        *this << data.sessionId;
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        *this << std::string_view(data.message, strnlen(data.message, sizeof(data.message)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(RoomChatReceivedPayload &data) -> Packet &
    {
        *this >> data.sessionId;

        std::string username;
        std::string message;
        *this >> username;
        *this >> message;

        std::strncpy(data.username, username.c_str(), sizeof(data.username));
        data.username[sizeof(data.username) - 1] = '\0';

        std::strncpy(data.message, message.c_str(), sizeof(data.message));
        data.message[sizeof(data.message) - 1] = '\0';

        return *this;
    }

    template <>
    inline auto Packet::operator<<(EntitySpawnPayload data) -> Packet &
    {
        *this << data.netId;
        *this << data.type;
        *this << data.posX;
        *this << data.posY;
        *this << data.sizeX;
        *this << data.sizeY;
        *this << data.weaponKind;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(EntitySpawnPayload &data) -> Packet &
    {
        *this >> data.netId;
        *this >> data.type;
        *this >> data.posX;
        *this >> data.posY;
        *this >> data.sizeX;
        *this >> data.sizeY;
        *this >> data.weaponKind;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(EntityDeathPayload data) -> Packet &
    {
        *this << data.netId;
        *this << data.type;
        *this << data.position.x;
        *this << data.position.y;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(EntityDeathPayload &data) -> Packet &
    {
        *this >> data.netId;
        *this >> data.type;
        *this >> data.position.x;
        *this >> data.position.y;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(AmmoUpdatePayload data) -> Packet &
    {
        *this << data.current;
        *this << data.max;
        *this << data.isReloading;
        *this << data.cooldownRemaining;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(AmmoUpdatePayload &data) -> Packet &
    {
        *this >> data.current;
        *this >> data.max;
        *this >> data.isReloading;
        *this >> data.cooldownRemaining;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(BeamStatePayload data) -> Packet &
    {
        *this << data.ownerNetId;
        *this << data.active;
        *this << data.timeRemaining;
        *this << data.length;
        *this << data.offsetY;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(BeamStatePayload &data) -> Packet &
    {
        *this >> data.ownerNetId;
        *this >> data.active;
        *this >> data.timeRemaining;
        *this >> data.length;
        *this >> data.offsetY;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(ScoreUpdatePayload data) -> Packet &
    {
        *this << data.score;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(ScoreUpdatePayload &data) -> Packet &
    {
        *this >> data.score;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(GameOverPayload data) -> Packet &
    {
        *this << std::string_view(data.bestPlayer, strnlen(data.bestPlayer, sizeof(data.bestPlayer)));
        *this << data.bestScore;
        *this << data.playerScore;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(GameOverPayload &data) -> Packet &
    {
        std::string bestPlayer;
        *this >> bestPlayer;
        std::strncpy(data.bestPlayer, bestPlayer.c_str(), sizeof(data.bestPlayer) - 1);
        data.bestPlayer[sizeof(data.bestPlayer) - 1] = '\0';
        *this >> data.bestScore;
        *this >> data.playerScore;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(HealthUpdatePayload data) -> Packet &
    {
        *this << data.current;
        *this << data.max;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(HealthUpdatePayload &data) -> Packet &
    {
        *this >> data.current;
        *this >> data.max;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(PingPayload data) -> Packet &
    {
        *this << data.clientTimeMs;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(PingPayload &data) -> Packet &
    {
        *this >> data.clientTimeMs;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(DebugModePayload data) -> Packet &
    {
        *this << data.enabled;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(DebugModePayload &data) -> Packet &
    {
        *this >> data.enabled;
        return *this;
    }

    // template <>
    // inline auto Packet::operator<<(WorldSnapshotPayload data) -> Packet &
    // {
    //     *this << data.serverTick;
    //     *this << data.entityCount;
    //     return *this;
    // }

    // template <>
    // inline auto Packet::operator>>(WorldSnapshotPayload &data) -> Packet &
    // {
    //     *this >> data.serverTick;
    //     *this >> data.entityCount;
    //     return *this;
    // }

    template <>
    inline auto Packet::operator<<(EntitySnapshotPayload data) -> Packet &
    {
        *this << data.netId;
        *this << data.position.x;
        *this << data.position.y;
        *this << data.velocity.x;
        *this << data.velocity.y;
        *this << data.rotation;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(EntitySnapshotPayload &data) -> Packet &
    {
        *this >> data.netId;
        *this >> data.position.x;
        *this >> data.position.y;
        *this >> data.velocity.x;
        *this >> data.velocity.y;
        *this >> data.rotation;
        return *this;
    }
} // namespace rtp::net
