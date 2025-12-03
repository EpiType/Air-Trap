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

namespace rtp::dl
{
    template <typename T>
    auto DynamicLibrary::getSymbol(std::string_view name) const
        -> std::expected<T, std::string>
    {
        if (!this->_handle)
            return std::unexpected{"Dynamic library handle is null"};

        auto symbol = impl::PlatformBackend::getSymbol(this->_handle, name);
        if (!symbol.has_value())
            return std::unexpected{std::format("Symbol '{}' not found"), name};

        return reinterpret_cast<T>(symbol.value());
    }
}
