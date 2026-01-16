/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryManager.hpp, declaration of dynamic library manager
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
 * @file LibraryManager.hpp
 * @brief Declaration of the Dynamic Library Manager.
 * This class is responsible for loading and managing dynamic libraries.
 * It provides APIs to load libraries in different modes:
 * - Main API: Load a library and return a raw pointer (observer).
 * - Shared API: Load a library and return a shared pointer (shared ownership).
 * - Isolated API: Load a library and return a unique pointer (unique ownership). 
 */

#ifndef ENGINE_LIBRARYMANAGER_HPP_
    #define ENGINE_LIBRARYMANAGER_HPP_

    #include "engine/core/Error.hpp"
    #include "engine/plugin/DynamicLibrary.hpp"

    #include <expected>
    #include <memory>
    #include <shared_mutex>
    #include <string>
    #include <string_view>
    #include <unordered_map>

namespace engine::plugin
{
    /**
     * @class LibraryManager
     * @brief Manages dynamic libraries,
     * providing loading and caching mechanisms.
     */
    class LibraryManager final {
        public:
            LibraryManager(void) = default;
            ~LibraryManager() noexcept = default;

            LibraryManager(const LibraryManager &) = delete;
            LibraryManager &operator=(const LibraryManager &) = delete;

            LibraryManager(LibraryManager &&) noexcept = delete;
            LibraryManager &operator=(LibraryManager &&) noexcept = delete;

            /**
             * @brief Main API.
             * Load a dynamic library, and return a raw pointer to the instance
             * (Observer).
             * @param path The path to the dynamic library.
             * @return std::expected<const DynamicLibrary *, engine::core::Error>
             */
            [[nodiscard]]
            auto load(std::string_view path)
                -> std::expected<const DynamicLibrary *, engine::core::Error>;

            /**
             * @brief Shared API.
             * Load a dynamic library from the given path.
             * If the library is already loaded, return the existing instance.
             * @param path The path to the dynamic library.
             * @return std::expected<std::shared_ptr<DynamicLibrary>,
             *                       engine::core::Error>
             */
            [[nodiscard]]
            auto loadShared(std::string_view path)
                -> std::expected<std::shared_ptr<DynamicLibrary>, engine::core::Error>;

            /**
             * @brief Isolated API (Factory).
             * Load a dynamic library, and create a unique instance of it,
             * out of the manager's cache nor tracking.
             * @param path The path to the dynamic library.
             * @return std::expected<std::unique_ptr<DynamicLibrary>,
             *                       engine::core::Error>
             */
            static auto loadStandalone(std::string_view path)
                -> std::expected<std::unique_ptr<DynamicLibrary>, engine::core::Error>;

        private:
            std::unordered_map<std::string,
                               std::shared_ptr<DynamicLibrary>> _libraries;
            std::shared_mutex _mutex; /**< Mutex for thread-safe access. */

        private:
            /**
             * @brief Get an existing library or load it if not present.
             * @param path The path to the dynamic library.
             * @return std::expected<std::shared_ptr<DynamicLibrary>,
             *                       engine::core::Error>
             */
            [[nodiscard]]
            auto getOrLoadInternal(std::string_view path)
                -> std::expected<std::shared_ptr<DynamicLibrary>,
                                 engine::core::Error>;

    };
}

#endif /* !ENGINE_LIBRARYMANAGER_HPP_ */
