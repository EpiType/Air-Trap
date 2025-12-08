/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Error.hpp, Error class declaration
*/

#include "Error.hpp"

namespace rtp
{           
    template <typename ...Args>
    auto Error::failure(ErrorCode code,
                        std::format_string<Args...> fmt,
                        Args &&...args) -> Error
    {
        return Error(code, log::Level::Error,
                     formatMessage(fmt, std::forward<Args>(args)...));
    }
            
    template <typename ...Args>
    auto Error::warning(ErrorCode code,
                        std::format_string<Args...> fmt,
                        Args &&...args) -> Error
    {
        return Error(code, log::Level::Warning,
                     formatMessage(fmt, std::forward<Args>(args)...));
    }
            
    template <typename ...Args>
    auto Error::fatal(ErrorCode code,
                      std::format_string<Args...> fmt,
                      Args &&...args) -> Error
    {
        return Error(code, log::Level::Fatal,
                     formatMessage(fmt, std::forward<Args>(args)...));
    }


}

auto std::formatter<rtp::Error>::format(const rtp::Error &e,
                                        format_context &ctx) const
{
}
