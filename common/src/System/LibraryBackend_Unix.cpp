/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryBackend_Unix.cpp, Unix based dynamic library handling
*/

/**
 * @file LibraryBackend_Unix.cpp
 * @brief Unix-based dynamic library handling implementation
 */

#include "RType/Assert.hpp"
#include "LibraryBackend.hpp"

#include <dlfcn.h>

namespace rtp::sys::impl
{
    auto LibraryBackend::open(std::string_view path)
        -> std::expected<void *, rtp::Error>
    {
        std::string safeNullTerminatedPath(path);
        void *handle = dlopen(safeNullTerminatedPath.c_str(),
                              RTLD_LAZY | RTLD_LOCAL);
        if (!handle) [[unlikely]] {
            const char *err = dlerror();
            return std::unexpected{Error::failure(ErrorCode::LibraryLoadFailed,
                                   "dlopen error: {}",
                                   err ? err : "Unknown dlopen error")};
        }

        return handle;
    }

    auto LibraryBackend::close(void *handle) noexcept
        -> std::expected<void, rtp::Error>
    {
        if (dlclose(handle) != 0) [[unlikely]] {
            const char *err = dlerror();
            if (err)
                return std::unexpected{
                    Error::failure(ErrorCode::LibraryLoadFailed,
                                   "dlclose error: {}",
                                   err ? err : "Unknown dlclose error")};
        }

        return {};
    }

    auto LibraryBackend::getSymbol(void *handle, std::string_view name)
        -> std::expected<void *, rtp::Error>
    {
        RTP_ASSERT(handle != nullptr,
                   "LoaderBackend: Handle cannot be null during symbol lookup");
        RTP_ASSERT(!name.empty(), "LoaderBackend: Symbol name cannot be empty");
        dlerror();

        std::string safeNullTerminatedName(name);
        void *symbol = dlsym(handle, safeNullTerminatedName.c_str());
        const char *err = dlerror();
        if (err) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::SymbolNotFound,
                                                  "dlsym error: {}", err)};

        return symbol;
    }
}
