/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** PlatformBackend_Mac.cpp, Unix based dynamic library handling
*/

/**
 * @file PlatformBackend_Unix.cpp
 * @brief Unix-based dynamic library handling implementation
 */

#include <dlfcn.h>

#include "Core/DynamicLibraries/PlatformBackend.hpp"

namespace rtp::dl::impl {
auto PlatformBackend::open(std::string_view path) noexcept -> std::expected<void*, std::string> {
    std::string safeNullTerminatedPath(path);
    void* handle = dlopen(safeNullTerminatedPath.c_str(), RTLD_LAZY | RTLD_LOCAL);
    if (!handle) {
        const char* err = dlerror();
        return std::unexpected{err ? err : "Unknown dlopen error"};
    }

    return handle;
}

auto PlatformBackend::close(void* handle) noexcept -> std::expected<void, std::string> {
    if (dlclose(handle) != 0) {
        const char* err = dlerror();
        return std::unexpected{err ? err : "Unknown dlclose error"};
    }

    return {};
}

auto PlatformBackend::getSymbol(void* handle, std::string_view name) noexcept -> std::expected<void*, std::string> {
    dlerror();

    std::string safeNullTerminatedName(name);
    void* symbol = dlsym(handle, safeNullTerminatedName.c_str());
    const char* err = dlerror();
    if (err)
        return std::unexpected{err};

    return symbol;
}
}  // namespace rtp::dl::impl
