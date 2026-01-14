/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
**
 * SystemConcept.hpp
*/

#ifndef RTYPE_SYSTEMCONCEPT_HPP_
#define RTYPE_SYSTEMCONCEPT_HPP_

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Signature.hpp"

#include <type_traits>

namespace rtp::ecs
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

#endif /* !RTYPE_SYSTEMCONCEPT_HPP_ */
