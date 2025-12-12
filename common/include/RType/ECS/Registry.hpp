/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.hpp
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
 * @file Registry.hpp
 * @brief Entity Component System registry implementation
 * @author Robin Toillon
 * @details Provides the central registry for managing entities and their
 * components in an ECS architecture. Supports thread-safe entity
 * creation, component registration, and efficient querying through
 * views.
 */

#ifndef RTYPE_REGISTRY_HPP_
    #define RTYPE_REGISTRY_HPP_

    #include "RType/ECS/ComponentConcept.hpp"
    #include "RType/ECS/Entity.hpp"
    #include "RType/ECS/SparseArray.hpp"
    #include "RType/Logger.hpp"

    #include <concepts>
    #include <deque>
    #include <expected>
    #include <memory>
    #include <mutex>
    #include <ranges>
    #include <span>
    #include <typeindex>
    #include <type_traits>
    #include <unordered_map>
    #include <utility>
    #include <vector>

namespace rtp::ecs
{
    /**
     * @brief Helper to copy const-ness from one type to another
     * @tparam From Source type to copy const-ness from
     * @tparam To Destination type to apply const-ness to
     */
    template <typename From, typename To>
    using ConstLike =
        std::conditional_t<std::is_const_v<std::remove_reference_t<From>>,
                           std::add_const_t<To>, To>;

    /**
     * @brief Helper to copy const-ness and create reference
     * @tparam From Source type to copy const-ness from
     * @tparam To Destination type to apply const-ness to
     */
    template <typename From, typename To>
    using ConstLikeRef = ConstLike<From, To> &;

    /**
     * @class Registry
     * @brief Central registry for managing entities and components in
     * ECS
     * @details The Registry is the core of the Entity Component System
     * architecture. It manages:
     * - Entity lifecycle (creation and destruction)
     * - Component storage and retrieval
     * - Entity-component associations
     * - Thread-safe access to all operations
     * 
     * The registry uses sparse arrays for efficient component storage
     * and supports entity recycling through generation counters.
     */
    class Registry {
        public:
            ~Registry() noexcept;

            /**
             * @brief Create a new entity
             * @return Expected containing the new entity or an error
             * @note Thread-safe. Recycles entity IDs when possible.
             */
            [[nodiscard]]
            auto spawnEntity(void) -> std::expected<Entity, rtp::Error>;

            /**
             * @brief Destroy an entity and all its components
             * @param entity The entity to destroy
             * @note Thread-safe. Invalidates the entity for recycling.
             */
            void killEntity(Entity entity);

            /**
             * @brief Register a component type with the registry
             * @tparam T The component type to register
             * @return Reference to the component storage array
             * @note Must be called before adding components of this type
             */
            template <Component T>
            [[nodiscard]]
            auto registerComponent(void) -> SparseArray<T> &;

            /**
             * @brief Get the storage array for a component type
             * @tparam T The component type
             * @tparam Self Deduced self type (const or non-const)
             * @return Expected containing reference to component array or error
             */
            template <Component T, typename Self>
            [[nodiscard]]
            auto getComponents(this Self &self)
                -> std::expected<ConstLikeRef<Self, SparseArray<T>>,
                                 rtp::Error>;

            /**
             * @brief Add a component to an entity
             * @tparam T The component type
             * @tparam Args Constructor argument types
             * @param entity The entity to add the component to
             * @param args Arguments to construct the component with
             * @return Expected containing reference to the new component or error
             */
            template <Component T, typename... Args>
            [[nodiscard]]
            auto addComponent(Entity entity, Args &&...args) &
                -> std::expected<T &, rtp::Error>;

            /**
             * @brief Create a view over all components of a specific type
             * @tparam T The component type
             * @tparam Self Deduced self type (const or non-const)
             * @return Span over all components of type T
             */
            template <Component T, typename Self>
            [[nodiscard]]
            auto view(this Self &self) -> std::span<ConstLike<Self, T>>;

            /**
             * @brief Create a zipped view over multiple component types
             * @tparam Ts The component types to zip together
             * @tparam Self Deduced self type (const or non-const)
             * @return Zipped view of components
             */
            template <Component... Ts, typename Self>
            [[nodiscard]]
            auto zipView(this Self &self);

        private:
            std::unordered_map<std::type_index,
                               std::unique_ptr<ISparseArray>> _arrays; /**< Component storage arrays */
            std::vector<std::uint32_t> _generations; /**< Entity generations for recycling */
            std::deque<std::uint32_t> _freeIndices;  /**< Recycled entity indices */
            std::mutex _mutex; /**< Mutex for thread safety */
    };
}

    #include "Registry.tpp" /* Registry implementation */

#endif /* !RTYPE_REGISTRY_HPP_ */
