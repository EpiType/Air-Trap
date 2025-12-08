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
    template <typename From, typename To>
    using ConstLike = std::copy_const_t<std::remove_reference_t<From>, To>;

    template <typename From, typename To>
    using ConstLikeRef = ConstLike<From, To> &;

    class Registry {
        public:
            ~Registry() noexcept;

            [[nodiscard]]
            auto spawnEntity(void) -> std::expected<Entity, rtp::Error>;

            void killEntity(Entity entity);

            template <Component T>
            [[nodiscard]]
            auto registerComponent(void) -> SparseArray<T> &;

            template <Component T, typename Self>
            [[nodiscard]]
            auto getComponents(this Self &&self)
                -> std::expected<ConstLikeRef<Self, SparseArray<T>>,
                                 rtp::Error>
            requires (std::is_lvalue_reference_v<Self>);

            template <Component T, typename... Args>
            [[nodiscard]]
            auto addComponent(Entity entity, Args &&...args) &
                -> std::expected<T &, rtp::Error>;

            template <Component T, typename Self>
            [[nodiscard]]
            auto view(this Self &&self) -> std::span<ConstLike<Self, T>>
            requires (std::is_lvalue_reference_v<Self>);

            template <Component... Ts, typename Self>
            [[nodiscard]]
            auto zipView(this Self &&self)
            requires (std::is_lvalue_reference_v<Self>);

        private:
            std::unordered_map<std::type_index,
                               std::unique_ptr<ISparseArray>> _arrays;

            std::vector<std::uint32_t> _generations; /**< Entity generations */
            std::deque<std::uint32_t> _freeIndices;  /**< Recycled entity indices */
            std::mutex _mutex; /**< Mutex for thread safety */
    };
}

#include "Registry.tpp" /* Registry implementation */

#endif /* !RTYPE_REGISTRY_HPP_ */
