/**
 * File   : Packet.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/Packet.hpp"

namespace rtp::net
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    aer::net::ByteBuffer Packet::serialize() const
    {
        Header net = header;
        net.magic = to_network(net.magic);
        net.sequenceId = to_network(net.sequenceId);
        net.bodySize = to_network(static_cast<uint32_t>(body.size()));
        net.ackId = to_network(net.ackId);
        net.sessionId = to_network(net.sessionId);

        aer::net::ByteBuffer data;
        data.resize(sizeof(Header) + body.size());
        std::memcpy(data.data(), &net, sizeof(Header));
        if (!body.empty()) {
            std::memcpy(data.data() + sizeof(Header), body.data(), body.size());
        }
        return data;
    }

    Packet Packet::deserialize(aer::net::ByteSpan data)
    {
        if (data.size() < sizeof(Header)) {
            throw std::runtime_error("Packet deserialize: buffer too small");
        }
        Header net{};
        std::memcpy(&net, data.data(), sizeof(Header));

        Packet packet;
        packet.header.magic = from_network(net.magic);
        packet.header.sequenceId = from_network(net.sequenceId);
        packet.header.bodySize = from_network(net.bodySize);
        packet.header.ackId = from_network(net.ackId);
        packet.header.opCode = net.opCode;
        packet.header.reserved = net.reserved;
        packet.header.sessionId = from_network(net.sessionId);

        if (packet.header.magic != MAGIC_NUMBER) {
            throw std::runtime_error("Packet deserialize: invalid magic");
        }
        const auto bodySize = static_cast<size_t>(packet.header.bodySize);
        if (data.size() < sizeof(Header) + bodySize) {
            throw std::runtime_error("Packet deserialize: truncated body");
        }
        packet.body.resize(bodySize);
        if (bodySize > 0) {
            std::memcpy(packet.body.data(), data.data() + sizeof(Header), bodySize);
        }
        return packet;
    }

    /////////////////////////////////////////////////////////////////////////
    // Private API
    /////////////////////////////////////////////////////////////////////////

    void Packet::_bumpBodySizeOrThrow(size_t delta)
    {
        if (body.size() + delta > MAX_BODY_SIZE) {
            throw std::runtime_error("Packet body too large");
        }
        body.resize(body.size() + delta);
        header.bodySize = static_cast<uint32_t>(body.size());
    }
} // namespace rtp::net
