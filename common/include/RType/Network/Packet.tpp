/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Packet.tpp
*/

#pragma once
#include <cstring>
#include <stdexcept>

namespace rtp::net
{
    ///////////////////////////////////////////////////////////////////////////
    // Template Implementations
    ///////////////////////////////////////////////////////////////////////////

    template <typename T>
    auto Packet::operator<<(T data) -> Packet &
    {
        T network_data = to_network(data);
        
        size_t dataSize = sizeof(T);
        size_t currentSize = body.size();
        body.resize(currentSize + dataSize);

        std::memcpy(body.data() + currentSize, &network_data, dataSize);
    
        return *this;
    }

    template <typename T>
    auto Packet::operator<<(const std::vector<T> &vec) -> Packet &
    {
        uint32_t vecSize = static_cast<uint32_t>(vec.size());
        *this << vecSize;
        for (const auto &item : vec) {
            *this << item;
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

        return *this;
    }

    template <typename T>
    auto Packet::operator>>(T &data) -> Packet &
    {
        if (_readPos + sizeof(T) > body.size()) {
            throw std::out_of_range("Packet read overflow");
        }

        T network_data;
        std::memcpy(&network_data, body.data() + _readPos, sizeof(T));
        _readPos += sizeof(T);

        data = from_network(network_data);

        return *this;
    }

    template <typename T>
    auto Packet::operator>>(std::vector<T> &vec) -> Packet &
    {
        uint32_t vecSize;
        *this >> vecSize;

        vec.clear();
        vec.reserve(vecSize);

        for (uint32_t i = 0; i < vecSize; ++i) {
            T item;
            *this >> item;
            vec.push_back(std::move(item));
        }
        return *this;
    }

    inline auto Packet::operator>>(std::string &str) -> Packet &
    {
        uint32_t strSize;
        *this >> strSize;
        if (_readPos + strSize > body.size()) {
            throw std::out_of_range("Packet read overflow");
        }
        str.assign(reinterpret_cast<const char *>(body.data() + _readPos), strSize);
        _readPos += strSize;
        return *this;
    }
    
     /**
     * @brief Converts a primitive type (integer, float) from Big-Endian (network) to machine endianness.
     * @note Uses std::byteswap for endianness conversion if necessary.
     * @param value The value to convert.
     * @return The value in machine endianness format.
     */
    template rtp::net::Packet& rtp::net::Packet::operator<<<uint32_t>(uint32_t data);
} // namespace rtp::net