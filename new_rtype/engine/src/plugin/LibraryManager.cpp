/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryManager.cpp, implementation of dynamic library manager
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
 * @file LibraryManager.cpp
 * @brief Implementation of the Dynamic Library Manager
 * @author Robin Toillon
 * @details This class is responsible for loading and managing dynamic
 * libraries. It provides APIs to load libraries in different modes:
 * - Main API: Load a library and return a raw pointer (observer).
 * - Shared API: Load a library and return a shared pointer (shared
 *   ownership).
 * - Isolated API: Load a library and return a unique pointer (unique
 *   ownership).
 */

#include "engine/plugin/LibraryManager.hpp"
#include "engine/plugin/LibraryBackend.hpp"

#include <mutex>

namespace aer::plugin
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    auto LibraryManager::load(std::string_view path)
        -> std::expected<const DynamicLibrary *, aer::log::Error>
    {
        return this->getOrLoadInternal(path).transform(
            [](const std::shared_ptr<DynamicLibrary> &libPtr) 
                -> const DynamicLibrary *
            {
                return libPtr.get();
            });
    }

    auto LibraryManager::loadShared(std::string_view path)
        -> std::expected<std::shared_ptr<DynamicLibrary>, aer::log::Error>
    {
        return this->getOrLoadInternal(path);
    }

    auto LibraryManager::loadStandalone(std::string_view path)
        -> std::expected<std::unique_ptr<DynamicLibrary>, aer::log::Error>
    {
        auto handle = impl::LibraryBackend::open(path);
        if (!handle.has_value()) [[unlikely]] {
            return std::unexpected{
                aer::log::Error::failure(
                    aer::log::ErrorCode::LibraryLoadFailed,
                    "Failed to load dynamic library at '{}': {}",
                    path, handle.error())};
        }

        return std::make_unique<DynamicLibrary>(handle.value());
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    auto LibraryManager::getOrLoadInternal(std::string_view path)
        -> std::expected<std::shared_ptr<DynamicLibrary>, aer::log::Error>
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

            auto handle = impl::LibraryBackend::open(path);
            if (!handle.has_value()) [[unlikely]] {
                return std::unexpected{
                    aer::log::Error::failure(
                        aer::log::ErrorCode::LibraryLoadFailed,
                        "Failed to load dynamic library at '{}': {}",
                        path, handle.error())};
            }

            auto lib = std::make_shared<DynamicLibrary>(handle.value());
            this->_libraries.emplace(std::string{path}, lib);

            return lib;
        }
    }
}
