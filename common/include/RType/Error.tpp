/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Error.tpp, Error class template implementation
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
 * @file Error.tpp
 * @brief Error class template implementations
 * @author Robin Toillon
 * @details Implements the factory methods for creating Error objects
 * with different severity levels (failure, warning, fatal) and
 * formatted messages.
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
                     std::format(fmt, std::forward<Args>(args)...));
    }
            
    template <typename ...Args>
    auto Error::warning(ErrorCode code,
                        std::format_string<Args...> fmt,
                        Args &&...args) -> Error
    {
        return Error(code, log::Level::Warning,
                     std::format(fmt, std::forward<Args>(args)...));
    }
            
    template <typename ...Args>
    auto Error::fatal(ErrorCode code,
                      std::format_string<Args...> fmt,
                      Args &&...args) -> Error
    {
        return Error(code, log::Level::Fatal,
                     std::format(fmt, std::forward<Args>(args)...));
    }
}
