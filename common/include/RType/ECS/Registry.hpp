/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.hpp
*/

#ifndef RTYPE_REGISTRY_HPP_
    #define RTYPE_REGISTRY_HPP_

    #include "RType/ECS/ComponentConcept.hpp"
    #include "RType/ECS/Entity.hpp"
    #include "RType/ECS/SparseArray.hpp"
    #include "RType/Logger.hpp"
    #include "RType/Error.hpp"

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

namespace rtp::ecs
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
            auto spawn(void) -> std::expected<Entity, rtp::Error>;

            void kill(Entity entity);

            template <Component T, typename Self>
            [[nodiscard]]
            auto registerComponent(this Self &self)
                -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                                  SparseArray<T>>>,
                                 rtp::Error>;

            template <Component T, typename Self>
            [[nodiscard]]
            auto getComponents(this Self &self)
                -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                                  SparseArray<T>>>,
                                 rtp::Error>;

            template <Component T, typename... Args>
            [[nodiscard]]
            auto addComponent(Entity entity, Args &&...args);

            [[nodiscard]]
            bool isAlive(Entity entity) const noexcept;

            template <Component T, typename... Args>
            [[nodiscard]]
            auto add(Entity entity, Args &&...args)
                -> std::expected<std::reference_wrapper<T>, rtp::Error>;

            template <Component T, typename Self>
            [[nodiscard]]
            auto get(this Self &self)
                -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                                  SparseArray<T>>>,
                                 rtp::Error>;

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


        private:
            std::unordered_map<std::type_index,
                               std::unique_ptr<ISparseArray>> _arrays; /**< Registered component arrays */
            std::vector<std::uint32_t> _generations; /**< Generation counters for entities */
            std::deque<std::size_t> _freeIndices; /**< Recyclable entity indices */
            std::shared_mutex _mutex; /**< Mutex for thread-safe operations */

        private:
            template <Component T, typename Self>
            [[nodiscard]]
            auto getArray(this Self &self)
                -> ConstLikeRef<Self, SparseArray<T>>;

            template <Component... Ts>
            [[nodiscard]]
            auto &getSmallestArray(void);

            template <Component... Ts>
            [[nodiscard]]
            bool hasAllComponents(Entity entity) const noexcept;

    };
}

#include "Registry.tpp"

#endif /* !RTYPE_REGISTRY_HPP_ */
