/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DynamicLibrary.hpp, declaration of dynamic library handling
*/

/**
 * @file DynamicLibrary.hpp
 * @brief Declaration of the DynamicLibrary class.
 * Represents a dynamically loaded library and provides methods
 * to retrieve symbols from it.
 */

#ifndef RTYPE_DYNAMICLIBRARY_HPP_
#define RTYPE_DYNAMICLIBRARY_HPP_

#include <expected>
#include <string>
#include <string_view>

namespace rtp::dl {
/**
 * @class DynamicLibrary
 * @brief Represents a dynamically loaded library.
 * Provides methods to retrieve symbols from the library.
 */
class DynamicLibrary final {
   public:
    /**
     * @brief Construct a DynamicLibrary with the given handle.
     * @param handle The handle to the dynamic library.
     * @note expect handle to be a valid pointer to an handler.
     */
    explicit DynamicLibrary(void* handle) noexcept;

    /**
     * @brief Destructor.
     * Closes the dynamic library.
     */
    ~DynamicLibrary() noexcept;

    DynamicLibrary(DynamicLibrary&& other) noexcept;
    DynamicLibrary& operator=(DynamicLibrary&& other) noexcept;

    DynamicLibrary(const DynamicLibrary&) = delete;
    DynamicLibrary& operator=(const DynamicLibrary&) = delete;

    /**
     * @brief Get a symbol from the dynamic library.
     * @tparam T The expected type of the symbol.
     * @param name The name of the symbol to retrieve.
     * @return std::expected<T, std::string>
     */
    template <typename T>
    [[nodiscard]]
    auto getSymbol(std::string_view name) const -> std::expected<T, std::string>;

   private:
    void* _handle{nullptr}; /**< The handle to the dynamic library. */
};
}  // namespace rtp::dl

#include "DynamicLibrary.tpp"

#endif /* !RTYPE_DYNAMICLIBRARY_HPP_ */
