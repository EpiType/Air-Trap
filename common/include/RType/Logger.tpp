/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Logger.tpp
*/

namespace rtp::log
{
    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    namespace detail
    {
        template <typename... Args>
        void safeLog(Level level, const std::source_location &loc, 
                     std::format_string<Args...> fmt, Args &&...args) noexcept
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

    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    template <typename... Args>
    void debug(LogFmt<std::type_identity_t<Args>...> wrapper,
               Args &&...args) noexcept
    {
        detail::safeLog(Level::Debug, wrapper.loc, wrapper.fmt,
                std::forward<Args>(args)...);
    }

    template <typename... Args>
    void info(LogFmt<std::type_identity_t<Args>...> wrapper,
              Args &&...args) noexcept
    {
        detail::safeLog(Level::Info, wrapper.loc, wrapper.fmt,
                std::forward<Args>(args)...);
    }

    template <typename... Args>
    void error(LogFmt<std::type_identity_t<Args>...> wrapper,
               Args &&...args) noexcept
    {
        detail::safeLog(Level::Error, wrapper.loc, wrapper.fmt,
                std::forward<Args>(args)...);
    }

    template <typename... Args>
    void warning(LogFmt<std::type_identity_t<Args>...> wrapper,
                 Args &&...args) noexcept
    {
        detail::safeLog(Level::Warning, wrapper.loc, wrapper.fmt,
                std::forward<Args>(args)...);
    }

    template <typename... Args>
    void fatal(LogFmt<std::type_identity_t<Args>...> wrapper,
               Args &&...args) noexcept
    {
        detail::safeLog(Level::Fatal, wrapper.loc, wrapper.fmt,
                std::forward<Args>(args)...);
    }
}
