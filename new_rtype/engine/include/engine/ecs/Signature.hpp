/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Signature.hpp
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
 * @file Signature.hpp
 * @brief Component signature representation for ECS
 * @author Robin Toillon
 * @details Defines the signature type used to identify which components
 * an entity possesses or a system requires.
 */

#ifndef ENGINE_SIGNATURE_HPP_
    #define ENGINE_SIGNATURE_HPP_

    #include <bitset>
    #include <cstdint>
    #include <cstddef>

namespace engine::ecs
{
    /**
     * @brief Maximum number of unique component types supported
     * 
     * This limit applies to the total number of different component types
     * that can be registered with the ECS.
     */
    constexpr size_t MAX_COMPONENTS = 64;

    /**
     * @typedef Signature
     * @brief Represents a signature for an entity's components in the
     * ECS architecture.
     * @details A Signature is implemented as a bitset where each bit
     * corresponds to a component type. If a bit is set to 1, it
     * indicates that the entity possesses that component type; if set
     * to 0, the entity does not have that component type. This allows
     * for efficient tracking and querying of component compositions for
     * entities within the ECS framework.
     * 
     * Signatures are also used by systems to specify which component
     * combinations they operate on.
     */
    using Signature = std::bitset<MAX_COMPONENTS>;
}

#endif /* !ENGINE_SIGNATURE_HPP_ */
