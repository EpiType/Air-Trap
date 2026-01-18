/**
 * File   : NetworkUtils.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_NET_NETWORKUTILS_HPP_
    #define ENGINE_NET_NETWORKUTILS_HPP_

    #include <cstdint>
    #include <span>
    #include <vector>

namespace aer::net
{
    using Byte = std::uint8_t;              /**< Definition of a byte type */
    using ByteBuffer = std::vector<Byte>;   /**< Dynamic buffer of bytes */
    using ByteSpan = std::span<const Byte>; /**< Non-owning view over a byte array */

    /**
     * @enum NetChannel
     * @brief Enumeration of network channels.
     */
    enum class NetChannel {
        TCP,
        UDP
    };

    /**
     * @struct NetworkEvent
     * @brief Represents a network event with session ID, payload, and channel.
     */
    struct NetworkEvent {
        uint32_t sessionId;
        ByteBuffer payload;
        NetChannel channel;
    };
}

#endif /* !ENGINE_NET_NETWORKUTILS_HPP_ */
