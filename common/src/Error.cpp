/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Error.cpp
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
 * @file Error.cpp
 * @brief Implementation of the Error handling system
 * @author Robin Toillon
 * @details Implements the RType error category for std::error_code
 * integration and error code conversion functions.
 */

#include "RType/Error.hpp"

namespace rtp
{
    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    namespace
    {
        class RTypeCategory : public std::error_category {
            public:
                const char *name(void) const noexcept override
                {
                    return "RTypeEngine";
                }

                std::string message(int ev) const override
                {
                    return std::string(toString(static_cast<ErrorCode>(ev)));
                }
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    const std::error_category &rtype_category(void) noexcept
    {
        static const RTypeCategory instance;
        return instance;
    }

    std::error_code make_error_code(ErrorCode e) noexcept
    {
        return {static_cast<int>(e), rtype_category()};
    }

}
