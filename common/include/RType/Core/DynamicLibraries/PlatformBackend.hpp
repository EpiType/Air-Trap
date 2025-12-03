/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PlatformBackend.hpp, declarations for platform-specific dynamic library handling
*/

/**
 * @file PlatformBackend.hpp
 * @brief Declarations for platform-specific dynamic library handling
 */

#ifndef RTYPE_PlatformBackend_HPP_
    #define RTYPE_PlatformBackend_HPP_

    #include <expected>
    #include <string>
    #include <string_view>

namespace rtp::dl::impl
{
    /**
     * @class PlatformBackend
     * @brief Provides platform-specific implementations
     * for dynamic library operations.
     */
    class PlatformBackend {
        public:
            /**
             * @brief Open a dynamic library from the given path.
             * @param path The path to the dynamic library.
             * @return std::expected<void *, std::string>
             */
            [[nodiscard]]
            static auto open(std::string_view path) noexcept
                -> std::expected<void *, std::string>;

            /**
             * @brief Close the dynamic library handle.
             * @param handle The handle to close.
             * @return std::expected<void, std::string>
             */
            [[nodiscard]]
            static auto close(void *handle) noexcept
                -> std::expected<void, std::string>;

            /**
             * @brief Get a symbol from the dynamic library.
             * @param handle The dynamic library handle.
             * @param name The name of the symbol to get.
             * @return std::expected<void *, std::string>
             */
            [[nodiscard]]
            static auto getSymbol(void *handle, std::string_view name) noexcept
                -> std::expected<void *, std::string>;
    };
}

#endif /* !RTYPE_PlatformBackend_HPP_ */