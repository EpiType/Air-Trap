/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryBackend.hpp, declarations for platform-specific dynamic
** library handling
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
 * @file LibraryBackend.hpp
 * @brief Declarations for platform-specific dynamic library handling
 * @author Robin Toillon
 * @details Defines the interface for platform-specific implementations
 * of dynamic library operations. Actual implementations are provided in
 * LibraryBackend_Unix.cpp and LibraryBackend_Win.cpp.
 */

#ifndef RTYPE_LIBRARYBACKEND_HPP_
    #define RTYPE_LIBRARYBACKEND_HPP_

    #include <expected>
    #include <string>
    #include <string_view>

    #include "LibraryBackend.hpp"

namespace rtp::sys::impl
{
    /**
     * @class LibraryBackend
     * @brief Provides platform-specific implementations for dynamic
     * library operations
     * @details This class encapsulates all platform-dependent code for
     * loading, unloading, and querying symbols from dynamic libraries.
     * It abstracts away the differences between Unix (dlopen) and
     * Windows (LoadLibrary) APIs.
     */
    class LibraryBackend final {
        public:
            /**
             * @brief Open a dynamic library from the given path.
             * @param path The path to the dynamic library.
             * @return std::expected<void *, rtp::Error>
             */
            [[nodiscard]]
            static auto open(std::string_view path)
                -> std::expected<void *, rtp::Error>;

            /**
             * @brief Close the dynamic library handle.
             * @param handle The handle to close.
             * @return std::expected<void, rtp::Error>
             */
            [[nodiscard]]
            static auto close(void *handle) noexcept
                -> std::expected<void, rtp::Error>;

            /**
             * @brief Get a symbol from the dynamic library.
             * @param handle The dynamic library handle.
             * @param name The name of the symbol to get.
             * @return std::expected<void *, rtp::Error>
             */
            [[nodiscard]]
            static auto getSymbol(void *handle, std::string_view name)
                -> std::expected<void *, rtp::Error>;
    };
}

#endif /* !RTYPE_LibraryBackend_HPP_ */