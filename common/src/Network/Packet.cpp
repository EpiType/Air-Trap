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
}