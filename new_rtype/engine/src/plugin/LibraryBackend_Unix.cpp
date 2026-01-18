/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryBackend_Unix.cpp, Unix based dynamic library handling
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
 * @file LibraryBackend_Unix.cpp
 * @brief Unix-based dynamic library handling implementation
 * @author Robin Toillon
 * @details Implements dynamic library operations using POSIX dlopen,
 * dlsym, and dlclose functions. Provides error handling using dlerror
 * for Unix-like systems.
 */

#include "engine/log/Assert.hpp"
#include "engine/plugin/LibraryBackend.hpp"

#include <dlfcn.h>

namespace aer::plugin::impl
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    auto LibraryBackend::open(std::string_view path)
        -> std::expected<void *, aer::core::Error>
    {
        std::string safeNullTerminatedPath(path);
        void *handle = dlopen(safeNullTerminatedPath.c_str(),
                              RTLD_LAZY | RTLD_LOCAL);
        if (!handle) [[unlikely]] {
            const char *err = dlerror();
            return std::unexpected{
                aer::core::Error::failure(
                    aer::core::ErrorCode::LibraryLoadFailed,
                    "dlopen error: {}",
                    err ? err : "Unknown dlopen error")};
        }

        return handle;
    }

    auto LibraryBackend::close(void *handle) noexcept
        -> std::expected<void, aer::core::Error>
    {
        if (dlclose(handle) != 0) [[unlikely]] {
            const char *err = dlerror();
            if (err) {
                return std::unexpected{
                    aer::core::Error::failure(
                        aer::core::ErrorCode::LibraryLoadFailed,
                        "dlclose error: {}",
                        err ? err : "Unknown dlclose error")};
            }
        }

        return {};
    }

    auto LibraryBackend::getSymbol(void *handle, std::string_view name)
        -> std::expected<void *, aer::core::Error>
    {
        RTP_ASSERT(handle != nullptr,
                   "LoaderBackend: Handle cannot be null during symbol lookup");
        RTP_ASSERT(!name.empty(), "LoaderBackend: Symbol name cannot be empty");
        dlerror();

        std::string safeNullTerminatedName(name);
        void *symbol = dlsym(handle, safeNullTerminatedName.c_str());
        const char *err = dlerror();
        if (err) [[unlikely]] {
            return std::unexpected{
                aer::core::Error::failure(
                    aer::core::ErrorCode::SymbolNotFound,
                    "dlsym error: {}", err)};
        }

        return symbol;
    }
}
