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

        T network_data{}; // FIX: init (évite warnings / propreté)
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

        // FIX: limite simple anti-DoS (cohérente avec vector)
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
    inline auto Packet::operator<<(PlayerConnectPayload data) -> Packet &
    {
        *this << data.sessionId;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(PlayerConnectPayload &data) -> Packet &
    {
        *this >> data.sessionId;
        return *this;
    }

    template <>
    inline auto Packet::operator<<(PlayerLoginPayload data) -> Packet &
    {
        *this << data.sessionId;
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        *this << std::string_view(data.password, strnlen(data.password, sizeof(data.password)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(PlayerLoginPayload &data) -> Packet &
    {
        std::string username;
        std::string password;
        *this >> data.sessionId;
        *this >> data.username;
        *this >> data.password;
        std::strncpy(data.username, username.c_str(), sizeof(data.username));
        data.username[sizeof(data.username) - 1] = '\0';
        std::strncpy(data.password, password.c_str(), sizeof(data.password));
        data.password[sizeof(data.password) - 1] = '\0';
        return *this;
    }

    template <>
    inline auto Packet::operator<<(PlayerRegisterPayload data) -> Packet &
    {
        *this << data.sessionId;
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        *this << std::string_view(data.password, strnlen(data.password, sizeof(data.password)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(PlayerRegisterPayload &data) -> Packet &
    {
        std::string username;
        std::string password;
        *this >> data.sessionId;
        *this >> data.username;
        *this >> data.password;
        std::strncpy(data.username, username.c_str(), sizeof(data.username));
        data.username[sizeof(data.username) - 1] = '\0';
        std::strncpy(data.password, password.c_str(), sizeof(data.password));
        data.password[sizeof(data.password) - 1] = '\0';
        return *this;
    }

    template <>
    inline auto Packet::operator<<(LoginResponsePayload data) -> Packet &
    {
        *this << data.sessionId;
        *this << data.success;
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(LoginResponsePayload &data) -> Packet &
    {
        *this >> data.sessionId;
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
        *this << data.sessionId;
        *this << data.success;
        *this << std::string_view(data.username, strnlen(data.username, sizeof(data.username)));
        return *this;
    }

    template <>
    inline auto Packet::operator>>(RegisterResponsePayload &data) -> Packet &
    {
        *this >> data.sessionId;
        *this >> data.success;
        std::string username;
        *this >> username;
        std::strncpy(data.username, username.c_str(), sizeof(data.username));
        data.username[sizeof(data.username) - 1] = '\0';
        return *this;
    }

    template <>
    inline auto Packet::operator<<(WorldSnapshotPayload data) -> Packet &
    {
        *this << data.serverTick;
        *this << data.entityCount;
        return *this;
    }

    template <>
    inline auto Packet::operator>>(WorldSnapshotPayload &data) -> Packet &
    {
        *this >> data.serverTick;
        *this >> data.entityCount;
        return *this;
    }

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
