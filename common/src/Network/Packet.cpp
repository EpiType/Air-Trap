/**
 * File   : Packet.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "RType/Network/Session.hpp"
#include "RType/Assert.hpp"
#include "RType/Logger.hpp"
#include "RType/Network/Packet.hpp"
#include <bit>
#include <cstring>
#include <iomanip>

using BufferSequence = std::array<asio::const_buffer, 2>;

namespace rtp::net
{
    Packet::Packet(OpCode op)
        : header{}, body{}, _readPos(0)
    {
        header.opCode = op;
        header.magic = MAGIC_NUMBER;
        header.sequenceId = 0;
        header.bodySize = 0;
        header.ackId = 0;
        header.reserved = 0;
    }

    void Packet::resetRead(void)
    {
        _readPos = 0;
    }

    BufferSequence Packet::getBufferSequence(void) const
    {
        _cacheHeader = header;

        _cacheHeader.magic = to_network(header.magic);
        _cacheHeader.sequenceId = to_network(header.sequenceId);
        _cacheHeader.bodySize = to_network(static_cast<uint32_t>(body.size()));
        _cacheHeader.ackId = to_network(header.ackId);
        _cacheHeader.sessionId  = to_network(_cacheHeader.sessionId);

        return {
            asio::const_buffer(&_cacheHeader, sizeof(Header)),
            asio::const_buffer(body.data(), body.size())
        };
    }
}