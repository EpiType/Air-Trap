/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Logger.hpp, Logger declaration
*/

/*
** MIT License
**
** Copyright (c) 2025 Robin Toillon
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * @file Logger.hpp
 * @brief Logger declaration with support for multiple log levels
 * @author Robin Toillon
 * @details Provides a thread-safe logging system with formatted output,
 * automatic source location tracking, and configurable output to both
 * console and file.
 */

#ifndef RTYPE_LOGGER_HPP_
    #define RTYPE_LOGGER_HPP_

    #include "RType/LogLevel.hpp"
    #include "RType/Error.hpp"

    #include <expected>
    #include <format>
    #include <source_location>
    #include <string>
    #include <string_view>
    #include <type_traits>
    #include <utility>

namespace rtp::log
{
    template <typename... Args>
    struct LogFmt {
        std::format_string<Args...> fmt;
        std::source_location loc;

        consteval LogFmt(const char *s,
                         std::source_location l =
                             std::source_location::current())
                        : fmt(s), loc(l)
        {
        }

        consteval LogFmt(std::string_view s,
                         std::source_location l =
                             std::source_location::current())
                        : fmt(s), loc(l)
        {
        }
    };

    /**
     * @brief Configure the logger using a configuration file
     * @param logFilePath The path to the configuration file
     * @return std::expected<void, rtp::Error>
     * @note If the configuration fails, returns an error message
     */
    [[nodiscard]]
    auto configure(std::string_view logFilePath) noexcept
        -> std::expected<void, rtp::Error>;

    /**
     * @brief Log a debug message
     * @tparam Args The types of the format arguments
     * @param fmt The format string
     * @param args The format arguments
     */
    template <typename... Args>
    void debug(LogFmt<std::type_identity_t<Args>...> fmt,
               Args &&...args) noexcept;

    /**
     * @brief Log an informational message
     * @tparam Args The types of the format arguments
     * @param fmt The format string
     * @param args The format arguments
     */
    template <typename... Args>
    void info(LogFmt<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept;

    /**
     * @brief Log an error message
     * @tparam Args The types of the format arguments
     * @param fmt The format string
     * @param args The format arguments
     */
    template <typename... Args>
    void error(LogFmt<std::type_identity_t<Args>...> fmt,
               Args &&...args) noexcept;

    /**
     * @brief Log a warning message
     * @tparam Args The types of the format arguments
     * @param fmt The format string
     * @param args The format arguments
     */
    template <typename... Args>
    void warning(LogFmt<std::type_identity_t<Args>...> fmt,
                 Args &&...args) noexcept;
    
    /**
     * @brief Log a fatal error message
     * @tparam Args The types of the format arguments
     * @param fmt The format string
     * @param args The format arguments
     */
    template <typename... Args>
    void fatal(LogFmt<std::type_identity_t<Args>...> fmt,
               Args &&...args) noexcept;

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    namespace detail
    {
        void logImpl(Level level, std::string_view message, 
                     const std::source_location &loc);
    }
}

    #include "Logger.tpp"

#endif /* !RTYPE_LOGGER_HPP_ */
