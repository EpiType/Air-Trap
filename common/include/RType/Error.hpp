/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Error.hpp, Error class declaration
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
 * @file Error.hpp
 * @brief Error handling system with categorized error codes
 * @author Robin Toillon
 * @details Provides a comprehensive error handling framework with:
 * - Categorized error codes (Core, Network, IO, ECS)
 * - Integration with std::error_code
 * - Severity levels (warning, failure, fatal)
 * - Formatted error messages
 * - Retry counting mechanism
 */

#ifndef RTYPE_CORE_ERROR_HPP_
    #define RTYPE_CORE_ERROR_HPP_

    #include "RType/LogLevel.hpp"

    #include <cstdint>
    #include <format>
    #include <string>
    #include <string_view>
    #include <system_error>
    #include <utility>

namespace rtp
{
    /**
     * @enum ErrorCode
     * @brief Enumeration of all possible error codes in the RType engine
     * @details Error codes are organized into categories:
     * - Core (1-99): General application errors
     * - Network (100-199): Network-related errors
     * - IO (200-299): File and library loading errors
     * - ECS (300-399): Entity Component System errors
     */
    enum class ErrorCode : std::uint16_t {
        Success = 0,
        
        // --- Core ---
        Unknown = 1,            /**< Unknown or unspecified error */
        InvalidParameter,       /**< Invalid function parameter provided */
        InternalRuntimeError,   /**< Internal runtime error */

        // --- Network ---
        ConnectionFailed = 100, /**< Failed to establish network connection */
        Disconnected,           /**< Connection was disconnected */
        Timeout,                /**< Network operation timed out */
        PayloadError,           /**< Error in network payload data */

        // --- IO
        FileNotFound = 200,     /**< Requested file does not exist */
        LibraryLoadFailed,      /**< Failed to load dynamic library */
        SymbolNotFound,         /**< Symbol not found in dynamic library */
        InvalidFormat,          /**< Invalid file or data format */

        // --- ECS ---
        ComponentMissing = 300, /**< Requested component not found on entity */
        RegistryFull,           /**< Entity registry has reached maximum capacity */
        EntityInvalid           /**< Entity identifier is invalid or stale */
    };

    /**
     * @brief Convert an ErrorCode to its string representation
     * @param e The error code to convert
     * @return String view containing the error code name
     */
    constexpr std::string_view toString(ErrorCode e) noexcept;

    /**
     * @brief Create a std::error_code from an ErrorCode
     * @param e The error code to convert
     * @return A std::error_code with the RType error category
     */
    std::error_code make_error_code(ErrorCode e) noexcept;
}

namespace std
{
    /**
     * @brief Specialization to enable ErrorCode as a valid error code enum
     * @details This allows ErrorCode to be used with std::error_code and
     * enables automatic conversion to std::error_code when needed.
     */
    template <>
    struct is_error_code_enum<rtp::ErrorCode> : true_type {};
}

namespace rtp {

    /**
     * @brief Get the RType error category for std::error_code
     * integration
     * @return Reference to the RType error category singleton
     */
    const std::error_category &rtype_category(void) noexcept;

    /**
     * @class Error
     * @brief Comprehensive error object with severity and retry tracking
     * @details The Error class provides a rich error representation that
     * includes:
     * - Error code from the ErrorCode enum
     * - Severity level (warning, failure, fatal)
     * - Formatted error message
     * - Retry counter for recoverable errors
     * 
     * Factory methods are provided to create errors with different
     * severity levels.
     */
    class Error {
        public:
            /**
             * @brief Create a failure-level error
             * @tparam Args Format argument types
             * @param code The error code
             * @param fmt Format string
             * @param args Format arguments
             * @return Error object with failure severity
             */
            template <typename ...Args>
            [[nodiscard]]
            static auto failure(ErrorCode code,
                                std::format_string<Args...> fmt,
                                Args &&...args) -> Error;
            
            /**
             * @brief Create a warning-level error
             * @tparam Args Format argument types
             * @param code The error code
             * @param fmt Format string
             * @param args Format arguments
             * @return Error object with warning severity
             */
            template <typename ...Args>
            [[nodiscard]]
            static auto warning(ErrorCode code,
                                std::format_string<Args...> fmt,
                                Args &&...args) -> Error;
            
            /**
             * @brief Create a fatal-level error
             * @tparam Args Format argument types
             * @param code The error code
             * @param fmt Format string
             * @param args Format arguments
             * @return Error object with fatal severity
             */
            template <typename ...Args>
            [[nodiscard]]
            static auto fatal(ErrorCode code,
                              std::format_string<Args...> fmt,
                              Args &&...args) -> Error;

            /**
             * @brief Get the severity level of this error
             * @return The log level representing error severity
             */
            log::Level severity(void) const noexcept;
            
            /**
             * @brief Get the current retry count
             * @return Number of times this error has been retried
             */
            std::uint8_t retryCount(void) const noexcept;
            
            /**
             * @brief Get the error message
             * @return String view of the formatted error message
             */
            auto message(void) const noexcept
                -> std::string_view;

            /**
             * @brief Increment the retry counter
             * @return The new retry count after incrementing
             */
            std::uint8_t incrementRetryCount(void) noexcept;

        private:
            ErrorCode _code;             /**< The error code */
            log::Level _severity;        /**< Severity level of the error */
            std::uint8_t _retryCount{0}; /**< Number of retry attempts */
            std::string _message;        /**< Formatted error message */

        private:
            /**
             * @brief Private constructor for error creation
             * @param code The error code
             * @param severity The severity level
             * @param msg The error message
             */
            Error(ErrorCode code, log::Level severity, std::string_view msg);

    };
}

/**
 * @brief Formatter specialization for Error objects
 * @details Allows Error objects to be used with std::format and
 * std::print.
 */
template <>
struct std::formatter<rtp::Error> : std::formatter<std::string> {
    /**
     * @brief Format an Error object
     * @param e The error to format
     * @param ctx The format context
     * @return Iterator to the end of the formatted output
     */
    auto format(const rtp::Error &e, format_context &ctx) const;
};

    #include "Error.inl" /* Inline/Constexpr implementations */
    #include "Error.tpp" /* Template implementations */

#endif /* !RTYPE_CORE_ERROR_HPP_ */
