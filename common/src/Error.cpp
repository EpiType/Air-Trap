/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Error.cpp
*/

#include "RType/Error.hpp"

namespace rtp
{
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
