/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Logger.hpp, Logger declaration
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
     * @brief Configure the logger using a configuration file.
     * @param logFilePath The path to the configuration file.
     * @return std::expected<void, rtp::Error>
     * @note If the configuration fails, returns an error message.
     */
    [[nodiscard]]
    auto configure(std::string_view logFilePath) noexcept
        -> std::expected<void, rtp::Error>;

    /**
     * @brief Log a debug message.
     * @tparam Args The types of the format arguments.
     * @param fmt The format string.
     * @param args The format arguments.
     * @param location The source location of the log call.
     */
    template <typename... Args>
    void debug(LogFmt<std::type_identity_t<Args>...> fmt,
               Args &&...args) noexcept;

    /**
     * @brief Log an informational message.
     * @tparam Args The types of the format arguments.
     * @param fmt The format string.
     * @param args The format arguments.
     * @param location The source location of the log call.
     */
    template <typename... Args>
    void info(LogFmt<std::type_identity_t<Args>...> fmt,
              Args &&...args) noexcept;

    /**
     * @brief Log an error message.
     * @tparam Args The types of the format arguments.
     * @param fmt The format string.
     * @param args The format arguments.
     */
    template <typename... Args>
    void error(LogFmt<std::type_identity_t<Args>...> fmt,
               Args &&...args) noexcept;

    /**
     * @brief Log a warning message.
     * @tparam Args The types of the format arguments.
     * @param fmt The format string.
     * @param args The format arguments.
     * @param location The source location of the log call.
     */
    template <typename... Args>
    void warning(LogFmt<std::type_identity_t<Args>...> fmt,
                 Args &&...args) noexcept;
    
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