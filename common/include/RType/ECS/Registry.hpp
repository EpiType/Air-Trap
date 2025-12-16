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

    #include <concepts>
    #include <deque>
    #include <expected>
    #include <functional>
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
            auto spawnEntity(void) -> std::expected<Entity, rtp::Error>;

            void killEntity(Entity entity);

            void clear() {
                std::lock_guard lock(_mutex);
                for (auto& [type, array] : _arrays) {
                    array->clear();
                }
                _generations.clear();
                _freeIndices.clear();
            }

            template <Component T>
            [[nodiscard]]
            auto registerComponent(void) -> SparseArray<T> &;

            template <Component T, typename Self>
            [[nodiscard]]
            auto getComponents(this Self &self)
                -> std::expected<std::reference_wrapper<ConstLike<Self, SparseArray<T>>>, rtp::Error>;

            template <Component T, typename... Args>
            [[nodiscard]]
            auto addComponent(Entity entity, Args &&...args)
                -> std::expected<std::reference_wrapper<T>, rtp::Error>;

            template <Component T, typename Self>
            [[nodiscard]]
            auto view(this Self &self) -> std::span<ConstLike<Self, T>>;

            template <Component... Ts, typename Self>
            [[nodiscard]]
            auto zipView(this Self &self);

        private:
            std::unordered_map<std::type_index,
                std::unique_ptr<ISparseArray>> _arrays;    /**< Registered component arrays */
            std::vector<std::uint32_t> _generations;       /**< Generation counters for entities */
            std::deque<std::uint32_t> _freeIndices;        /**< Recyclable entity indices */
            std::mutex _mutex;                             /**< Mutex for thread-safe operations */
    };
}

#include "Registry.tpp"

#endif /* !RTYPE_REGISTRY_HPP_ */