/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Logger.tpp
*/

#include "Logger.hpp"

namespace rtp::log
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    template <typename... Args>
    void info(std::format_string<Args...> &&fmt, Args &&...args,
              const std::source_location location) noexcept
    {
        safeLog(Level::Info, location, fmt,
                 std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(std::format_string<Args...> &&fmt, Args &&...args,
               const std::source_location location) noexcept
    {
        safeLog(Level::Error, location, fmt,
                 std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warning(std::format_string<Args...> &&fmt, Args &&...args,
                 const std::source_location location) noexcept
    {
        safeLog(Level::Warning, location, fmt,
                 std::forward<Args>(args)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    namespace detail
    {
        template <typename... Args>
        void safeLog(Level level, const std::source_location& loc, 
                     std::format_string<Args...> fmt, Args&&... args) noexcept
        {
            try {
                std::string msg = std::format(fmt,
                                              std::forward<Args>(args)...);
                logImpl(level, msg, loc);
            } catch (...) {
                logImpl(Level::Error,
                               "LOGGER FORMAT ERROR (OOM?)", loc);
            }
        }
    }
}
