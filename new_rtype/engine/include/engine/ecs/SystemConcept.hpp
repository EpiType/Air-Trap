/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
**
 * SystemConcept.hpp
*/

#ifndef ENGINE_SYSTEMCONCEPT_HPP_
#define ENGINE_SYSTEMCONCEPT_HPP_

#include "engine/ecs/ISystem.hpp"
#include "engine/ecs/Signature.hpp"

#include <type_traits>

namespace engine::ecs
{
    /**
     * @brief Concept defining the requirements for a System type
     *
     * @details A System must inherit from ISystem and implement an update

     * * method that takes a float delta time parameter.
     */
    template <typename T>
    concept System = std::is_base_of_v<ISystem, T> &&
                     requires {
                         { T::getRequiredSignature() }
                             -> std::same_as<Signature>;
    };
}

#endif /* !ENGINE_SYSTEMCONCEPT_HPP_ */
