/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LibraryManager.hpp, declaration of dynamic library manager
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

#ifndef RTYPE_LIBRARYMANAGER_HPP_
    #define RTYPE_LIBRARYMANAGER_HPP_

    #include "RType/System/DynamicLibrary.hpp"

    #include <expected>
    #include <memory>
    #include <shared_mutex>
    #include <string>
    #include <string_view>
    #include <unordered_map>

namespace rtp::sys
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
             * @return std::expected<const DynamicLibrary *, std::string>
             */
            [[nodiscard]]
            auto load(std::string_view path)
                -> std::expected<const DynamicLibrary *, std::string>;

            /**
             * @brief Shared API.
             * Load a dynamic library from the given path.
             * If the library is already loaded, return the existing instance.
             * @param path The path to the dynamic library.
             * @return std::expected<std::shared_ptr<DynamicLibrary>,
             *                       std::string>
             */
            [[nodiscard]]
            auto loadShared(std::string_view path)
                -> std::expected<std::shared_ptr<DynamicLibrary>, std::string>;

            /**
             * @brief Isolated API (Factory).
             * Load a dynamic library, and create a unique instance of it,
             * out of the manager's cache nor tracking.
             * @param path The path to the dynamic library.
             * @return std::expected<std::unique_ptr<DynamicLibrary>,
             *                       std::string>
             */
            static auto loadStandalone(std::string_view path)
                -> std::expected<std::unique_ptr<DynamicLibrary>, std::string>;

        private:
            std::unordered_map<std::string,
                               std::shared_ptr<DynamicLibrary>> _libraries;
            std::shared_mutex _mutex; /**< Mutex for thread-safe access. */

        private:
            /**
             * @brief Get an existing library or load it if not present.
             * @param path The path to the dynamic library.
             * @return std::expected<std::shared_ptr<DynamicLibrary>,
             *                       std::string>
             */
            [[nodiscard]]
            auto getOrLoadInternal(std::string_view path)
                -> std::expected<std::shared_ptr<DynamicLibrary>,
                                 std::string>;

    };
}

#endif /* !RTYPE_LIBRARYMANAGER_HPP_ */