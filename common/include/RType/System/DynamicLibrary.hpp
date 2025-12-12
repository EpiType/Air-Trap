/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DynamicLibrary.hpp, declaration of dynamic library handling
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
 * @file DynamicLibrary.hpp
 * @brief Declaration of the DynamicLibrary class.
 * Represents a dynamically loaded library and provides methods
 * to retrieve symbols from it.
 */

#ifndef RTYPE_DYNAMICLIBRARY_HPP_
    #define RTYPE_DYNAMICLIBRARY_HPP_

    #include "RType/Error.hpp"

    #include <expected>
    #include <string>
    #include <string_view>

namespace rtp::sys
{
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
            explicit DynamicLibrary(void *handle) noexcept;

            /** 
             * @brief Destructor.
             * Closes the dynamic library.
             */
            ~DynamicLibrary() noexcept;
        
            DynamicLibrary(DynamicLibrary &&other) noexcept;
            DynamicLibrary &operator=(DynamicLibrary &&other) noexcept;

            DynamicLibrary(const DynamicLibrary &) = delete;
            DynamicLibrary &operator=(const DynamicLibrary &) = delete;

            /**
             * @brief Get a symbol from the dynamic library.
             * @tparam T The expected type of the symbol.
             * @param name The name of the symbol to retrieve.
             * @return std::expected<T, rtp::Error>
             */
            template <typename T>
            [[nodiscard]]
            auto get(std::string_view name) const
                -> std::expected<T, rtp::Error>;

        private:
            void *_handle{nullptr}; /**< The handle to the dynamic library. */
        
            /**
             * @brief Get the address of a symbol from the dynamic library.
             * @param name The name of the symbol to retrieve.
             * @return std::expected<void *, rtp::Error>
             */
            [[nodiscard]]
            auto getSymbolAddress(std::string_view name) const 
                -> std::expected<void *, rtp::Error>;

    };
}

    #include "DynamicLibrary.tpp"

#endif /* !RTYPE_DYNAMICLIBRARY_HPP_ */
