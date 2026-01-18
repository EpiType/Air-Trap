/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ComponentConcept.hpp
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
 * @file ComponentConcept.hpp
 * @brief Concepts for validating component types in the ECS
 * @author Robin Toillon
 * @details Defines compile-time constraints for types that can be used
 * as components in the Entity Component System.
 */

#ifndef RTYPE_COMPONENTCONCEPT_HPP_
    #define RTYPE_COMPONENTCONCEPT_HPP_

    #include <concepts>
    #include <type_traits>
    #include <cstddef>

namespace aer::ecs
{
    /**
     * @concept Component
     * @brief Concept for valid ECS component types
     * @tparam T The type to check
     * @details A type is a valid component if it is:
     * - A class type
     * - Move constructible
     * - Destructible
     */
    template <typename T>
    concept Component = std::is_class_v<T> && 
                        std::move_constructible<T> &&
                        std::destructible<T>;

    /**
     * @concept Serializable
     * @brief Concept for network-serializable components
     * @tparam T The type to check
     * @details A type is serializable if it is:
     * - A valid Component
     * - Standard layout (predictable memory layout)
     * - Trivially copyable (can be memcpy'd)
     * 
     * Serializable components can be efficiently transmitted over the
     * network.
     */
    template <typename T>
    concept Serializable = Component<T> &&
                           std::is_standard_layout_v<T> &&
                           std::is_trivially_copyable_v<T>;

    inline std::size_t nextComponentID() {
        static std::size_t counter = 0;
        return counter++;
    }

    template <typename T>
    std::size_t getStaticComponentID() {
        // Cette variable 'id' est unique pour CHAQUE type T.
        // Elle n'est initialisée qu'une seule fois au premier appel de getStaticComponentID<T>().
        static std::size_t id = nextComponentID();
        return id;
   }
}

#endif /* !RTYPE_COMPONENTCONCEPT_HPP_ */
