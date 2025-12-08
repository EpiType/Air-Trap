/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Error.inl
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
            case EntityInvalid:
                return "Entity invalid";

            default:
                std::unreachable();
        }
    }
}
