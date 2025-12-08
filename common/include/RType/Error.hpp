/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Error.hpp, Error class declaration
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
    enum class ErrorCode : std::uint16_t {
        Success = 0,
        
        // --- Core ---
        Unknown = 1,
        InvalidParameter,

        // --- Network ---
        ConnectionFailed = 100,
        Disconnected,
        Timeout,
        PayloadError,

        // --- IO
        FileNotFound = 200,
        LibraryLoadFailed,
        SymbolNotFound,
        InvalidFormat,

        // --- ECS ---
        ComponentMissing = 300,
        EntityInvalid
    };

    constexpr std::string_view toString(ErrorCode e) noexcept;

    std::error_code make_error_code(ErrorCode e) noexcept;
}

namespace std
{

    template <>
    struct is_error_code_enum<rtp::ErrorCode> : true_type {};
}

namespace rtp {

    const std::error_category &rtype_category(void) noexcept;

    class Error {
        public:
            
            template <typename ...Args>
            [[nodiscard]]
            static auto failure(ErrorCode code,
                                std::format_string<Args...> fmt,
                                Args &&...args) -> Error;
            
            template <typename ...Args>
            [[nodiscard]]
            static auto warning(ErrorCode code,
                                std::format_string<Args...> fmt,
                                Args &&...args) -> Error;
            
            template <typename ...Args>
            [[nodiscard]]
            static auto fatal(ErrorCode code,
                                std::format_string<Args...> fmt,
                                Args &&...args) -> Error;

            static auto fromSystem(ErrorCode code,
                                   std::string_view prefix) -> Error;

            log::Level severity(void) const noexcept;
            std::uint8_t retryCount(void) const noexcept;
            auto message(void) const noexcept
                -> std::string_view;

            std::uint8_t incrementRetryCount(void) noexcept;

        private:
            ErrorCode _code;
            log::Level _severity;
            std::uint8_t _retryCount{0};
            std::string _message;

        private:
            Error(ErrorCode code, log::Level severity, std::string_view msg);

    };
}

template <>
struct std::formatter<rtp::Error> : std::formatter<std::string> {
    auto format(const rtp::Error &e, format_context &ctx) const;
};

    #include "Error.inl" /* Inline/Constexpr implementations */
    #include "Error.tpp" /* Template implementations */

#endif /* !RTYPE_CORE_ERROR_HPP_ */
