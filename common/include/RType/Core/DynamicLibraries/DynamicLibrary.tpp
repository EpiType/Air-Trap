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
#include "../../../../src/Core/DynamicLibraries/PlatformBackend.hpp" // To fix include path

namespace rtp::dl
{
    template <typename T>
    auto DynamicLibrary::getSymbol(std::string_view name) const
        -> std::expected<T, std::string>
    {
        if (!this->_handle)
            return std::unexpected{"Dynamic library handle is null"};

        /* ========== To check ============ */
        auto result = impl::PlatformBackend::getSymbol(this->_handle, name);
        // void *symbol = impl::PlatformBackend::getSymbol(this->_handle, name);
       
        if (!result)
            return std::unexpected{std::format("Failed to get symbol '{}' : '{}'", name, result.error())};
        void *symbol = *result;
        /* ================================ */
        
        if (!symbol)
            return std::unexpected{std::format("Symbol '{}' not found", name)};

        return reinterpret_cast<T>(symbol);
    }
}
