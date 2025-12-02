/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Logger.hpp, Logger declaration
*/

#ifndef RTYPE_LOGGER_HPP_
    #define RTYPE_LOGGER_HPP_

    #include <expected>
    #include <format>
    #include <source_location>
    #include <string>
    #include <string_view>

namespace rtp::log
{
    enum class Level {
        Debug,
        Info,
        Warning,
        Error
    };

    /**
     * @brief Configure the logger using a configuration file.
     * @param logFilePath The path to the configuration file.
     * @return std::expected<void, std::string>
     * @note If the configuration fails, returns an error message.
     */
    [[nodiscard]]
    auto configure(std::string_view logFilePath) noexcept
        -> std::expected<void, std::string>;

    /**
     * @brief Log an informational message.
     * @tparam Args The types of the format arguments.
     * @param fmt The format string.
     * @param args The format arguments.
     * @param location The source location of the log call.
     */
    template <typename... Args>
    void info(std::format_string<Args...> fmt, Args &&...args,
              const std::source_location location =
                  std::source_location::current()) noexcept;

    /**
     * @brief Log an error message.
     * @tparam Args The types of the format arguments.
     * @param fmt The format string.
     * @param args The format arguments.
     * @param location The source location of the log call.
     */
    template <typename... Args>
    void error(std::format_string<Args...> fmt, Args &&...args,
               const std::source_location location =
                   std::source_location::current()) noexcept;

    /**
     * @brief Log a warning message.
     * @tparam Args The types of the format arguments.
     * @param fmt The format string.
     * @param args The format arguments.
     * @param location The source location of the log call.
     */
    template <typename... Args>
    void warning(std::format_string<Args...> fmt, Args &&...args,
                 const std::source_location location =
                     std::source_location::current()) noexcept;

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
