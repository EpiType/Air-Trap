/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Packet.tpp
*/

namespace rtp::net
{
    template <typename T>
    inline T Packet::to_network(T v)
    {
        if constexpr (std::is_enum_v<T>) {
            using U = std::underlying_type_t<T>;
            return static_cast<T>(to_network(static_cast<U>(v)));
        } else if constexpr (std::is_integral_v<T>) {
            if constexpr (sizeof(T) > 1 && NATIVE_ENDIAN == std::endian::little) {
                return std::byteswap(v);
            }
            return v;
        } else if constexpr (std::is_same_v<T, float>) {
            if constexpr (NATIVE_ENDIAN == std::endian::little) {
                uint32_t u = std::bit_cast<uint32_t>(v);
                u = std::byteswap(u);
                return std::bit_cast<float>(u);
            }
            return v;
        }
        return v;
    }

    template <typename T>
    inline T Packet::from_network(T value)
    {
        return to_network(value);
    }

    template <typename T>
    inline Packet &Packet::operator<<(const T &data)
    {
        static_assert(std::is_trivially_copyable_v<T> || std::is_enum_v<T>,
                      "Packet::operator<< supports trivially copyable or enum types");
        const T net = to_network(data);
        _bumpBodySizeOrThrow(sizeof(T));
        const size_t i = body.size() - sizeof(T);
        std::memcpy(body.data() + i, &net, sizeof(T));
        return *this;
    }

    template <typename T>
    inline Packet &Packet::operator<<(const std::vector<T> &vec)
    {
        if (vec.size() > MAX_VECTOR_SIZE) {
            throw std::runtime_error("Packet::operator<< vector too large");
        }
        *this << static_cast<uint32_t>(vec.size());
        for (const auto &value : vec) {
            *this << value;
        }
        return *this;
    }

    template <typename T>
    inline Packet &Packet::operator>>(T &data)
    {
        static_assert(std::is_trivially_copyable_v<T> || std::is_enum_v<T>,
                      "Packet::operator>> supports trivially copyable or enum types");
        if (_readPos + sizeof(T) > body.size()) {
            throw std::runtime_error("Packet::operator>> out of bounds");
        }
        std::memcpy(&data, body.data() + _readPos, sizeof(T));
        data = from_network(data);
        _readPos += sizeof(T);
        return *this;
    }

    template <typename T>
    inline Packet &Packet::operator>>(std::vector<T> &vec)
    {
        uint32_t size = 0;
        *this >> size;
        if (size > MAX_VECTOR_SIZE) {
            throw std::runtime_error("Packet::operator>> vector too large");
        }
        vec.resize(size);
        for (auto &value : vec) {
            *this >> value;
        }
        return *this;
    }
}
