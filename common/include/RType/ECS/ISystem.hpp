/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ISystem.hpp
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
 * @file ISystem.hpp
 * @brief Interface for ECS systems
 * @author Robin Toillon
 * @details Defines the base interface that all systems in the Entity
 * Component System must implement. Systems contain the game logic that
 * operates on entities with specific component combinations.
 */

#ifndef RTYPE_ISYSTEM_HPP_
    #define RTYPE_ISYSTEM_HPP_

    #include "RType/ECS/Registry.hpp"

namespace rtp::ecs
{
    /**
     * @class ISystem
     * @brief Abstract base class for all ECS systems
     * @details Systems implement the logic that operates on entities
     * with specific component combinations. Each system should query the
     * registry for entities matching its requirements and update their
     * components.
     */
    class ISystem {
        public:
            virtual ~ISystem() noexcept = default;

            /**
             * @brief Update system logic for one frame
             * @param registry The entity registry to operate on
             * @param deltaTime Time elapsed since last update in seconds
             */
            virtual void update(Registry &registry, float deltaTime) = 0;
    };
}

#endif /* !RTYPE_ISYSTEM_HPP_ */
