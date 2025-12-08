/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DynamicLibrary.cpp, implementation of dynamic library handling
*/

#include "RType/System/DynamicLibrary.hpp"
#include "LibraryBackend.hpp"

namespace rtp::sys
{
    DynamicLibrary::DynamicLibrary(void *handle) noexcept : _handle(handle)
    {
    }

    DynamicLibrary::~DynamicLibrary() noexcept
    {
        if (this->_handle) [[unlikely]]
            impl::LibraryBackend::close(this->_handle);
    }

    DynamicLibrary::DynamicLibrary(DynamicLibrary &&other) noexcept
                                  : _handle(other._handle)
    {
        other._handle = nullptr;
    }

    DynamicLibrary &DynamicLibrary::operator=(DynamicLibrary &&other) noexcept
    {
        if (this != &other) {
            if (this->_handle) [[unlikely]]
                impl::LibraryBackend::close(this->_handle);
            this->_handle = other._handle;
            other._handle = nullptr;
        }

        return *this;
    }

    auto DynamicLibrary::getSymbolAddress(std::string_view name) const 
        -> std::expected<void *, std::string>
    {
        return impl::LibraryBackend::getSymbol(this->_handle, name);
    }
}
