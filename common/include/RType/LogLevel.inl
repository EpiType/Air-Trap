/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LogLevel.inl
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
 * @file LogLevel.inl
 * @brief Inline implementations for LogLevel.hpp
 */

namespace rtp::log
{
    constexpr std::string_view toString(Level level) noexcept
    {
        switch (level) {
            using enum Level;
            case Level::Debug:
                return "Debug";
            case Level::Info:
                return "Info";
            case Level::Warning:
                return "Warning";
            case Level::Error:
                return "Error";
            case Level::Fatal:
                return "Fatal";
            case Level::None:
                return "None";
            default:
                std::unreachable();
        }
    }
}

auto std::formatter<rtp::log::Level>::format(rtp::log::Level level,
                                             std::format_context &ctx) const
{
    return std::format_to(ctx.out(), "{}", rtp::log::toString(level));
}
