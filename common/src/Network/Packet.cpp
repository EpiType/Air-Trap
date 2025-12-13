/**
 * File   : Packet.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "RType/Network/Core/Session.hpp"
#include "RType/Assert.hpp"
#include "RType/Logger.hpp"
#include "RType/Network/Packet.hpp"
#include <bit>
#include <cstring>

using BufferSequence = std::array<asio::const_buffer, 2>;

namespace rtp::net
{
    /**
     * @brief Native endianness of the machine
     */
    constexpr std::endian NATIVE_ENDIAN = std::endian::native;

    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    template <typename T>
    inline T Packet::to_network(T value)
    {
        if constexpr (sizeof(T) > 1 && NATIVE_ENDIAN == std::endian::little) {
            return std::byteswap(value); 
        }
        return value;
    }

    template <typename T>
    inline T Packet::from_network(T value)
    {
        return to_network(value);
    }


    void Packet::resetRead(void)
    {
        _readPos = 0;
    };

    BufferSequence Packet::getBufferSequence(void) const
    {
        Header tempHeader = header;

        tempHeader.bodySize = to_network(static_cast<uint32_t>(body.size()));

        tempHeader.magic = to_network(MAGIC_NUMBER);
        
        std::array<asio::const_buffer, 2> buffers = {
            asio::const_buffer(&tempHeader, sizeof(Header)),
            asio::const_buffer(body.data(), body.size())
        };
        return buffers;
    }

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

    auto Packet::operator<<(std::string_view str) -> Packet &
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

    auto Packet::operator>>(std::string &str) -> Packet &
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
}