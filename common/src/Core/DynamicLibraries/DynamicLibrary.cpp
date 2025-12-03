/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** DynamicLibrary.cpp, implementation of dynamic library handling
*/

#include "DynamicLibrary.hpp"
#include "PlatformBackend.hpp"

namespace rtp::dl
{
    DynamicLibrary::DynamicLibrary(void *handle) noexcept : _handle(handle)
    {
    }

    DynamicLibrary::~DynamicLibrary() noexcept
    {
        if (this->_handle) [[unlikely]]
            impl::PlatformBackend::close(this->_handle);
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
                impl::PlatformBackend::close(this->_handle);
            this->_handle = other._handle;
            other._handle = nullptr;
        }

        return *this;
    }
}
