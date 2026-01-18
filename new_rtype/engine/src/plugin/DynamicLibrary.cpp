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
 * @file DynamicLibrary.cpp
 * @brief Implementation of platform-independent dynamic library wrapper
 * @author Robin Toillon
 * @details Provides a RAII wrapper around platform-specific dynamic
 * library handles, managing the lifecycle of loaded libraries and
 * symbol resolution.
 */

#include "engine/log/Logger.hpp"
#include "engine/plugin/DynamicLibrary.hpp"
#include "engine/plugin/LibraryBackend.hpp"

namespace aer::plugin
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    DynamicLibrary::DynamicLibrary(void *handle) noexcept : _handle(handle)
    {
    }

    DynamicLibrary::~DynamicLibrary() noexcept
    {
        if (this->_handle) [[likely]] {
            auto result = impl::LibraryBackend::close(this->_handle);
            if (!result.has_value()) {
                aer::log::warning("Failed to close dynamic library: {}",
                                      result.error().message());
            }
        }
    }

    DynamicLibrary::DynamicLibrary(DynamicLibrary &&other) noexcept
                                  : _handle(other._handle)
    {
        other._handle = nullptr;
    }

    DynamicLibrary &DynamicLibrary::operator=(DynamicLibrary &&other) noexcept
    {
        if (this != &other) {
            if (this->_handle) [[unlikely]] {
                auto result = impl::LibraryBackend::close(this->_handle);
                if (!result.has_value()) {
                    aer::log::warning("Failed to close dynamic library: {}",
                                          result.error().message());
                }
            }
            this->_handle = other._handle;
            other._handle = nullptr;
        }

        return *this;
    }

    auto DynamicLibrary::getSymbolAddress(std::string_view name) const 
        -> std::expected<void *, aer::core::Error>
    {
        return impl::LibraryBackend::getSymbol(this->_handle, name);
    }
}
