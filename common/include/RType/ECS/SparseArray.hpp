/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SparseArray.hpp
*/

#ifndef RTYPE_SPARSEARRAY_HPP_
    #define RTYPE_SPARSEARRAY_HPP_

    #include "RType/Assert.hpp"
    #include "RType/ECS/ComponentConcept.hpp"
    #include "RType/ECS/Entity.hpp"

namespace rtp::ecs
{
    class ISparseArray { // Only for type erasure
        public:
            virtual ~ISparseArray() noexcept = default;
            virtual void erase(Entity entity) = 0;
            virtual bool has(Entity entity) const = 0;
            virtual void clear(void) = 0;
    };

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

            void erase(Entity entity) noexcept override final;

            [[nodiscard]]
            bool has(Entity entity) const noexcept override final;

            void clear(void) noexcept override final;

            template <typename Self>
            [[nodiscard]]
            auto &&operator[](this Self &&self, Entity entity) noexcept
                requires (std::is_lvalue_reference_v<Self>);

            template <typename... Args>
            [[nodiscard]]
            auto emplace(Entity entity, Args &&...args) & -> T &;

            template <typename Self>
            [[nodiscard]]
            auto &&getData(this Self &&self) noexcept
                requires (std::is_lvalue_reference_v<Self>);

            template <typename Self>
            [[nodiscard]]
            auto &&getEntities(this Self &&self) noexcept
                requires (std::is_lvalue_reference_v<Self>);

            template <typename Self>
            [[nodiscard]]
            auto begin(this Self &&self) noexcept
                requires (std::is_lvalue_reference_v<Self>);

            template <typename Self>
            [[nodiscard]]
            auto end(this Self &&self) noexcept
                requires (std::is_lvalue_reference_v<Self>);

            [[nodiscard]]
            std::size_t size(void) const noexcept;

            [[nodiscard]]
            bool empty(void) const noexcept;

        private:
            std::vector<size_t> _sparse; /**< The Sparse Array (The Map) */
            std::vector<Entity> _dense; /** The Dense Entity Array
                                        (The Reverse Lookup) */
            container_t _data; /**< The Dense Component Array
                               (The Cache Friendly Data) */
    };
}

    #include "SparseArray.tpp" /* SparseArray implementation */

#endif /* !RTYPE_SPARSEARRAY_HPP_ */
