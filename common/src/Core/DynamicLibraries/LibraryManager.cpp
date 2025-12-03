/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryManager.cpp, implementation of dynamic library manager
*/

/**
 * @file LibraryManager.cpp
 * @brief Implementation of the Dynamic Library Manager.
 * This class is responsible for loading and managing dynamic libraries.
 * It provides APIs to load libraries in different modes:
 * - Main API: Load a library and return a raw pointer (observer).
 * - Shared API: Load a library and return a shared pointer (shared ownership).
 * - Isolated API: Load a library and return a unique pointer (unique ownership). 
 */

#include "RType/Core/DynamicLibraries/LibraryManager.hpp"
#include "RType/Core/DynamicLibraries/PlatformBackend.hpp"

#include <format>
#include <mutex>

namespace rtp::dl
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    auto LibraryManager::load(std::string_view path) noexcept
        -> std::expected<const DynamicLibrary *, std::string>
    {
        return this->getOrLoadInternal(path).transform(
            [](const std::shared_ptr<DynamicLibrary> &libPtr) 
                -> const DynamicLibrary *
            {
                return libPtr.get();
            });
    }

    auto LibraryManager::loadShared(std::string_view path) noexcept
        -> std::expected<std::shared_ptr<DynamicLibrary>, std::string>
    {
        return this->getOrLoadInternal(path);
    }

    auto LibraryManager::loadStandalone(std::string_view path) noexcept
        -> std::expected<std::unique_ptr<DynamicLibrary>, std::string>
    {
        auto handle = impl::PlatformBackend::open(path);
        if (!handle.has_value()) [[unlikely]]
            return std::unexpected{
                std::format("Failed to load dynamic library at '{}': '{}'",
                            path, handle.error())};

        return std::make_unique<DynamicLibrary>(handle.value());
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    auto LibraryManager::getOrLoadInternal(std::string_view path) noexcept
        -> std::expected<std::shared_ptr<DynamicLibrary>, std::string>
    {
        const std::string pathStr{path}; // TODO: avoid copy, by adding HashStringView
                                         //       as key in unordered_map

        // --- READ LOCK --- (Cache hit)
        {
            std::shared_lock lock{this->_mutex};
            auto it = this->_libraries.find(pathStr);
            if (it != this->_libraries.end()) [[likely]]
                return it->second;
        }
        // --- WRITE LOCK --- (Cache miss)
        {
            std::unique_lock lock{this->_mutex};
            auto it = this->_libraries.find(pathStr);
            if (it != this->_libraries.end()) [[unlikely]]
                return it->second;

            auto handle = impl::PlatformBackend::open(path);
            if (!handle.has_value()) [[unlikely]]
                return std::unexpected{
                    std::format("Failed to load dynamic library at '{}': '{}'",
                                path, handle.error())};

            auto lib = std::make_shared<DynamicLibrary>(handle.value());
            this->_libraries.emplace(std::string{path}, lib);

            return lib;
        }
    }
}