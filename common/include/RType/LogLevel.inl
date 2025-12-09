/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LogLevel.inl
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
