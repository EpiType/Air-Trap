/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SystemManager.hpp
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
 * @file SystemManager.hpp
 * @brief Manager for ECS systems
 * @author Robin Toillon
 * @details Provides centralized management of all systems in the ECS
 * architecture, including registration, signature management, and
 * coordinated updates.
 */

#ifndef RTYPE_SYSTEMMANAGER_HPP_
    #define RTYPE_SYSTEMMANAGER_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "RType/ECS/Signature.hpp"
    #include <memory>
    #include <unordered_map>
    #include <typeindex>


namespace rtp::ecs
{
    /**
     * @class SystemManager
     * @brief Manages all systems in the ECS architecture
     * @details The SystemManager is responsible for:
     * - Registering system instances
     * - Associating component signatures with systems
     * - Coordinating system updates each frame
     * - Managing system lifecycle
     */
    class SystemManager {
    public:
        /**
         * @brief Register a new system
         * @tparam T The system type to register
         * @note The system type must derive from ISystem
         */
        template <typename T>
        void registerSystem(void);

        /**
         * @brief Get a registered system instance
         * @tparam T The system type to retrieve
         * @return Reference to the system instance
         */
        template <typename T>
        auto getSystem(void) -> T &;

        /**
         * @brief Set the component signature for a system
         * @tparam T The system type
         * @param signature Bitset indicating which components the system
         * requires
         */
        template <typename T>
        void setSignature(Signature signature);

        /**
         * @brief Update all registered systems
         * @param registry The entity registry
         * @param dt Delta time since last update in seconds
         */
        void update(Registry &registry, float dt);

    private:
        Registry *_registry{nullptr};  ///< Pointer to the entity registry

        std::unordered_map<std::type_index,
                           std::unique_ptr<ISystem>> _systems; ///< Registered systems
        std::unordered_map<std::type_index, Signature> _signatures; ///< System signatures
    };
}

#endif /* !RTYPE_SYSTEMMANAGER_HPP_ */
