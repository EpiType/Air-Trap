/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryBackend_Win.cpp, Windows-specific dynamic library handling
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
 * @file LibraryBackend_Win.cpp
 * @brief Windows-specific dynamic library handling implementation
 * @author Robin Toillon
 * @details Implements dynamic library operations using Windows API
 * functions LoadLibraryA, GetProcAddress, and FreeLibrary. Provides
 * error handling using GetLastError for Windows systems.
 */

#if defined(_WIN32) || defined(_WIN64)
#include "RType/Assert.hpp"
#include "LibraryBackend.hpp"

#include <system_error>
#include <windows.h>

namespace rtp::dl::impl
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

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

#endif /* Windows platform */