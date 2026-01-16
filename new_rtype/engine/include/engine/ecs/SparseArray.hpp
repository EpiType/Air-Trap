/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SparseArray.hpp
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
 * @file SparseArray.hpp
 * @brief Sparse array container for efficient component storage in ECS
 * @author Robin Toillon
 * @details Provides a sparse array data structure optimized for storing
 * components indexed by entity IDs. Uses a sparse-to-dense mapping for
 * efficient iteration while maintaining O(1) access by entity.
 */

#ifndef ENGINE_SPARSEARRAY_HPP_
    #define ENGINE_SPARSEARRAY_HPP_

    #include "engine/core/Assert.hpp"
    #include "engine/ecs/ComponentConcept.hpp"
    #include "engine/ecs/Entity.hpp"

    #include <vector>
    #include <limits>

namespace engine::ecs
{
    /**
     * @class ISparseArray
     * @brief Type-erased base interface for sparse arrays
     * @details Allows storing sparse arrays of different component types
     * in a uniform container through type erasure.
     */
    class ISparseArray { // Only for type erasure
        public:
            virtual ~ISparseArray() noexcept = default;
            
            /**
             * @brief Remove a component from an entity
             * @param entity The entity whose component to remove
             */
            virtual void erase(Entity entity) = 0;
            
            /**
             * @brief Check if an entity has a component
             * @param entity The entity to check
             * @return true if the entity has this component
             */
            virtual bool has(Entity entity) const = 0;
            
            /**
             * @brief Remove all components
             */
            virtual void clear(void) = 0;
    };

    /**
     * @class SparseArray
     * @brief Sparse array container for component storage
     * @tparam T The component type to store
     * @details Implements an efficient sparse set data structure that:
     * - Provides O(1) component access by entity ID
     * - Allows efficient iteration over all components
     * - Maintains entity-to-component mapping
     * - Supports component addition, removal, and querying
     */
    template <Component T>
    class SparseArray final : public ISparseArray {
        public:
            using value_type = T;
            using container_t = std::vector<value_type>;
        
            static constexpr std::size_t NullIndex =
                std::numeric_limits<size_t>::max();

            SparseArray() = default;
            SparseArray(const SparseArray &) = default;
            SparseArray(SparseArray &&) noexcept = default;
            SparseArray &operator=(const SparseArray &) = default;
            SparseArray &operator=(SparseArray &&) noexcept = default;
            ~SparseArray() override = default;

            /**
             * @brief Remove a component from an entity
             * @param entity The entity whose component to remove
             */
            void erase(Entity entity) noexcept override final;

            /**
             * @brief Check if an entity has this component
             * @param entity The entity to check
             * @return true if the entity has this component
             */
            [[nodiscard]]
            bool has(Entity entity) const noexcept override final;

            /**
             * @brief Remove all components
             */
            void clear(void) noexcept override final;

            /**
             * @brief Access component by entity
             * @tparam Self Deduced self type (const or non-const)
             * @param entity The entity to get the component for
             * @return Reference to the component
             * @note Asserts if the entity does not have this component
             * If the entity does not have the component, behavior is undefined
             */
            template <typename Self>
            [[nodiscard]]
            auto &&operator[](this Self &self, Entity entity) noexcept;

            /**
             * @brief Construct a component in-place for an entity
             * @tparam Args Constructor argument types
             * @param entity The entity to add the component to
             * @param args Arguments to construct the component with
             * @return Reference to the newly created component
             */
            template <typename... Args>
            T &emplace(Entity entity, Args &&...args);

            /**
             * @brief Get the underlying dense component array
             * @tparam Self Deduced self type (const or non-const)
             * @return Reference to the dense component container
             */
            template <typename Self>
            [[nodiscard]]
            auto data(this Self &&self) noexcept;

            /**
             * @brief Get the entity array corresponding to components
             * @tparam Self Deduced self type (const or non-const)
             * @return Reference to the entity container
             */
            template <typename Self>
            [[nodiscard]]
            auto entities(this Self &&self) noexcept;

            /**
             * @brief Get the number of components stored
             * @return Number of components
             */
            [[nodiscard]]
            std::size_t size(void) const noexcept;

            /**
             * @brief Check if the array is empty
             * @return true if no components are stored
             */
            [[nodiscard]]
            bool empty(void) const noexcept;

        private:
            std::vector<size_t> _sparse;   /**< The Sparse Array (The Map) */
            std::vector<Entity> _dense;    /**< The Dense Entity Array (The Reverse Lookup) */
            container_t _data;             /**< The Dense Component Array (The Cache Friendly Data) */
    };
}

    #include "SparseArray.tpp" /* SparseArray implementation */

#endif /* !ENGINE_SPARSEARRAY_HPP_ */
