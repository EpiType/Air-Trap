/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Logger.tpp
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
 * @file Logger.tpp
 * @brief Logger template implementations
 * @author Robin Toillon
 * @details Implements the templated logging functions (debug, info,
 * error, warning, fatal) with format string support and automatic
 * source location capture.
 */

namespace engine::core
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
