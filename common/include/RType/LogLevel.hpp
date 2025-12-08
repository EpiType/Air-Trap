/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LogLevel.hpp, LogLevel enumeration declaration
*/

#ifndef RTYPE_LOGLEVEL_HPP_
    #define RTYPE_LOGLEVEL_HPP_

    #include <cstdint>

namespace rtp::log
{
    enum class Level : std::uint8_t {
        Debug = 0,
        Info,
        Warning,
        Error,
        Fatal,
        None
    };

}

#endif /* !RTYPE_LOGLEVEL_HPP_ */