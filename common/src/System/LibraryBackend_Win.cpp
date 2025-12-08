/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryBackend_Win.cpp, Windows-specific dynamic library handling
*/

/**
 * @file LibraryBackend_Win.cpp
 * @brief Windows-specific dynamic library handling implementation
 */

#include "RType/Assert.hpp"
#include "LibraryBackend.hpp"

#include <system_error>
#include <windows.h>

namespace rtp::dl::impl
{
    auto LibraryBackend::open(std::string_view path)
        -> std::expected<void *, rtp::Error>
    {
        std::string safeNullTerminatedPath(path);
        HMODULE handle = LoadLibraryA(safeNullTerminatedPath.c_str());
        if (!handle) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::LibraryLoadFailed,
                "LoadLibrary error: {}",
                std::system_error{}.message(GetLastError()))};

        return reinterpret_cast<void *>(handle);
    }

    auto LibraryBackend::close(void *handle) noexcept
        -> std::expected<void, rtp::Error>
    {
        if (!FreeLibrary(reinterpret_cast<HMODULE>(handle))) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::LibraryLoadFailed,
                                                  "FreeLibrary error: {}",
                                                  std::system_error{}.message(
                                                      GetLastError()))};

        return {};
    }

    auto LibraryBackend::getSymbol(void *handle, std::string_view name)
        -> std::expected<void *, rtp::Error>
    {
        RTP_ASSERT(handle != nullptr,
                   "LoaderBackend: Handle cannot be null during symbol lookup");
        RTP_ASSERT(!name.empty(), "LoaderBackend: Symbol name cannot be empty");

        std::string safeNullTerminatedName(name);
        FARPROC symbol = GetProcAddress(reinterpret_cast<HMODULE>(handle),
                                        safeNullTerminatedName.c_str());
        if (!symbol) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::SymbolNotFound,
                                                  "GetProcAddress error: {}",
                                                  std::system_error{}.message(
                                                      GetLastError()))};

        return reinterpret_cast<void *>(symbol);
    }
}
