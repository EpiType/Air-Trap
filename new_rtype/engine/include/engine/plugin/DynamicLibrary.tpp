/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DynamicLibrary.cpp, implementation of dynamic library handling
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
 * @file DynamicLibrary.tpp
 * @brief Implementation of the DynamicLibrary template methods.
 * @author Robin Toillon
 */

namespace engine::plugin
{
    template <typename T>
    auto DynamicLibrary::get(std::string_view name) const
        -> std::expected<T, engine::core::Error>
    {
        if (!this->_handle)
            return std::unexpected{
                engine::core::Error::failure(
                    engine::core::ErrorCode::LibraryLoadFailed,
                    "Dynamic library handle is null")};

        auto symbol = this->getSymbolAddress(name);
        if (!symbol.has_value())
            return std::unexpected{symbol.error()};

        return reinterpret_cast<T>(symbol.value());
    }
}
