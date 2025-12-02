/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PlatformBackend_Win.cpp, Windows-specific dynamic library handling
*/

/**
 * @file PlatformBackend_Win.cpp
 * @brief Windows-specific dynamic library handling implementation
 */

#include "Core/DynamicLibraries/PlatformBackend.hpp"

#include <system_error>
#include <windows.h>

namespace rtp::dl::impl
{
    auto PlatformBackend::open(std::string_view path) noexcept
        -> std::expected<void *, std::string>
    {
        std::string safeNullTerminatedPath(path);
        HMODULE handle = LoadLibraryA(safeNullTerminatedPath.c_str());
        if (!handle)
            return std::unexpected(
                std::system_error{}.message(GetLastError()));

        return reinterpret_cast<void *>(handle);
    }

    auto PlatformBackend::close(void *handle) noexcept
        -> std::expected<void, std::string>
    {
        if (!FreeLibrary(reinterpret_cast<HMODULE>(handle)))
            return std::unexpected{
                std::system_error{}.message(GetLastError())};

        return {};
    }

    auto PlatformBackend::getSymbol(void *handle,
                                    std::string_view name) noexcept
        -> std::expected<void *, std::string>
    {
        std::string safeNullTerminatedName(name);
        FARPROC symbol = GetProcAddress(reinterpret_cast<HMODULE>(handle),
                                        safeNullTerminatedName.c_str());
        if (!symbol)
            return std::unexpected{
                std::system_error{}.message(GetLastError())};

        return reinterpret_cast<void *>(symbol);
    }
}
