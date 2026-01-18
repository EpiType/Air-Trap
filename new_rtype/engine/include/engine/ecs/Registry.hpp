/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.hpp
*/

#ifndef ENGINE_REGISTRY_HPP_
    #define ENGINE_REGISTRY_HPP_

    #include "engine/ecs/ComponentConcept.hpp"
    #include "engine/ecs/Entity.hpp"
    #include "engine/ecs/SparseArray.hpp"
    #include "engine/ecs/ZipView.hpp"
    #include "engine/ecs/Signature.hpp"
    #include "engine/log/Logger.hpp"
    #include "engine/log/Error.hpp"

    #include <array>
    #include <concepts>
    #include <deque>
    #include <expected>
    #include <functional>
    #include <memory>
    #include <mutex>
    #include <ranges>
    #include <shared_mutex>
    #include <span>
    #include <tuple>
    #include <typeindex>
    #include <type_traits>
    #include <unordered_map>
    #include <utility>
    #include <vector>

namespace aer::ecs
{
    template <typename From, typename To>
    using ConstLike =
        std::conditional_t<std::is_const_v<std::remove_reference_t<From>>,
                           std::add_const_t<To>, To>;

    template <typename From, typename To>
    using ConstLikeRef = ConstLike<From, To> &;

    class Registry {
        public:
            ~Registry() noexcept = default;

            [[nodiscard]]
            auto spawn(void) -> std::expected<Entity, aer::core::Error>;

            void kill(Entity entity);

            template <Component T, typename Self>
            auto registerComponent(this Self &self)
                -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                                  SparseArray<T>>>,
                                 aer::core::Error>;

            template <Component T, typename Self>
            auto getComponents(this Self &self)
                -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                                  SparseArray<T>>>,
                                 aer::core::Error>;

            [[nodiscard]]
            bool isAlive(Entity entity) const noexcept;

            template <Component T, typename... Args>
            auto add(Entity entity, Args &&...args)
                -> std::expected<std::reference_wrapper<T>, aer::core::Error>;

            template <Component T, typename Self>
            [[nodiscard]]
            auto get(this Self &self)
                -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                                  SparseArray<T>>>,
                                 aer::core::Error>;

            template <Component T>
            [[nodiscard]]
            bool has(Entity entity) const noexcept;

            template <Component T>
            void remove(Entity entity) noexcept;

            void clear(void) noexcept;

            void purge(void) noexcept;

            template <Component... Ts, typename Self>
            [[nodiscard]]
            auto view(this Self &self);

            template <Component... Ts, typename Self>
            [[nodiscard]]
            auto zipView(this Self &self);

            [[nodiscard]]
            std::size_t entityCount(void) const noexcept;

            template <Component... Ts>
            [[nodiscard]]
            std::size_t componentCount(void) const noexcept;

            [[nodiscard]]
            bool anyMatch(const Signature &signature) const noexcept;

        private:
            std::unordered_map<std::type_index,
                               std::unique_ptr<ISparseArray>> _arrays; /**< Registered component arrays */
            std::vector<std::uint32_t> _generations; /**< Generation counters for entities */
            std::deque<std::size_t> _freeIndices; /**< Recyclable entity indices */
            std::vector<Signature> _entitySignatures; /**< Per-entity component signatures */
            mutable std::shared_mutex _mutex; /**< Mutex for thread-safe operations */

        private:
            template <Component T, typename Self>
            [[nodiscard]]
            auto getArray(this Self &self)
                -> ConstLikeRef<Self, SparseArray<T>>;

            template <Component T, Component... Ts>
            [[nodiscard]]
            auto &getSmallestArray(void);

            template <Component... Ts>
            [[nodiscard]]
            bool hasAllComponents(Entity entity) const noexcept;

    };
}

#include "Registry.tpp"

#endif /* !ENGINE_REGISTRY_HPP_ */
