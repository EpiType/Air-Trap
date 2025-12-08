/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ComponentConcept.hpp
*/

#ifndef RTYPE_COMPONENTCONCEPT_HPP_
    #define RTYPE_COMPONENTCONCEPT_HPP_

    #include <concepts>
    #include <type_traits>

namespace rtp::ecs
{
    template <typename T>
    concept Component = std::is_class_v<T> && 
                        std::move_constructible<T> &&
                        std::destructible<T>;

    template <typename T>
    concept Serializable = Component<T> &&
                           std::is_standard_layout_v<T> &&
                           std::is_trivially_copyable_v<T>;
}

#endif /* !RTYPE_COMPONENTCONCEPT_HPP_ */
