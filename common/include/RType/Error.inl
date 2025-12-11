/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Error.inl
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
 * @file Error.inl
 * @brief Error inline implementations
 * @author Robin Toillon
 * @details Implements the constexpr toString function that converts
 * ErrorCode enums to their string representations for human-readable
 * error messages.
 */

#include "Error.hpp"

namespace rtp
{
    constexpr std::string_view toString(ErrorCode e) noexcept
    {
        switch (e) {
            using enum ErrorCode;
            case Success:
                return "Success";

        // --- Core ---
            case Unknown:
                return "Unknown error";
            case InvalidParameter:
                return "Invalid parameter";
            case InternalRuntimeError:
                return "Internal runtime error";

        // --- Network ---
            case ConnectionFailed:
                return "Connection failed";
            case Disconnected:
                return "Disconnected";
            case Timeout:
                return "Timeout occurred";
            case PayloadError:
                return "Payload error";

        // --- IO
            case FileNotFound:
                return "File not found";
            case LibraryLoadFailed:
                return "Library load failed";
            case SymbolNotFound:
                return "Symbol not found";
            case InvalidFormat:
                return "Invalid format";

        // --- ECS ---
            case ComponentMissing:
                return "Component missing";
            case RegistryFull:
                return "Registry full";
            case EntityInvalid:
                return "Entity invalid";

            default:
                std::unreachable();
        }
    }
}

inline auto std::formatter<rtp::ErrorCode>::format(rtp::ErrorCode e,
                                            std::format_context &ctx) const
{
    return std::formatter<std::string_view>::format(rtp::toString(e), ctx);
}
