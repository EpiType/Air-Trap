/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryBackend.hpp, declarations for platform-specific dynamic library handling
*/

/**
 * @file LibraryBackend.hpp
 * @brief Declarations for platform-specific dynamic library handling
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
     * @brief Provides platform-specific implementations
     * for dynamic library operations.
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