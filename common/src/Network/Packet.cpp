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
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    Packet::Packet(OpCode op)
        : header{}, body{}, _readPos(0)
    {
        header.opCode = op;
        header.magic = MAGIC_NUMBER;
        header.sequenceId = 0;
        header.bodySize = 0;
        header.ackId = 0;
        header.reserved = 0;
    };


    void Packet::resetRead(void)
    {
        _readPos = 0;
    };

    BufferSequence Packet::getBufferSequence(void) const
    {
        Header tempHeader = header;

        tempHeader.magic = to_network(header.magic);
        tempHeader.sequenceId = to_network(header.sequenceId);
        tempHeader.bodySize = to_network(static_cast<uint32_t>(body.size()));
        tempHeader.ackId = to_network(header.ackId);

        // ==== what ?? it only work when the logs are enabled ??? ====
        std::stringstream ss;
        ss << "Sending Header (BE Hex): ";
        const uint8_t* byte_ptr = reinterpret_cast<const uint8_t*>(&tempHeader);
        
        for (size_t i = 0; i < sizeof(Header); ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << (int)byte_ptr[i] << " ";
        }
        rtp::log::debug("Sending packet: OpCode={} (0x{:02x}), BodySize={}",
            (int)tempHeader.opCode,
            (int)tempHeader.opCode,
            rtp::net::Packet::from_network(tempHeader.bodySize)
        );
        // ============================================================
        
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

    /**
     * @brief Converts a primitive type (integer, float) from Big-Endian (network) to machine endianness.
     * @note Uses std::byteswap for endianness conversion if necessary.
     * @param value The value to convert.
     * @return The value in machine endianness format.
     */
    template rtp::net::Packet& rtp::net::Packet::operator<<<uint32_t>(uint32_t data);
}