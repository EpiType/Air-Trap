/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DynamicLibrary.cpp, implementation of dynamic library handling
*/

/**
 * @file DynamicLibrary.tpp
 * @brief Implementation of the DynamicLibrary template methods.
 */

#include <format>

namespace rtp::sys
{
    template <typename T>
    auto DynamicLibrary::get(std::string_view name) const
        -> std::expected<T, rtp::Error>
    {
        if (!this->_handle)
            return std::unexpected{
                Error::failure(ErrorCode::LibraryLoadFailed,
                               "Dynamic library handle is null")};

        auto symbol = this->getSymbolAddress(name);
        if (!symbol.has_value())
            return std::unexpected{symbol.error()};

        return reinterpret_cast<T>(symbol.value());
    }
}
