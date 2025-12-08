/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.hpp
*/

#ifndef RTYPE_REGISTRY_HPP_
    #define RTYPE_REGISTRY_HPP_

    #include <concepts>
    #include <deque>
    #include <expected>
    #include <memory>
    #include <mutex>
    #include <span>
    #include <ranges>
    #include <typeindex>
    #include <unordered_map>
    #include <utility>
    #include <vector>

    #include "RType/Logger.hpp"
    #include "RType/ECS/ComponentConcept.hpp"
    #include "RType/ECS/Entity.hpp"
    #include "RType/ECS/SparseArray.hpp"

namespace rtp::ecs
{
    template <typename From, typename To>
    using ConstLike = std::conditional_t<std::is_const_v<
                                             std::remove_reference_t<From>>,
                                         std::add_const_t<To>, To>;

    class Registry {
        public:

            ~Registry() noexcept;

            [[nodiscard]]
            auto spawnEntity(void) -> std::expected<Entity, std::string>;

            void killEntity(Entity entity);

            template <Component T>
            [[nodiscard]]
            auto registerComponent(void) -> SparseArray<T> &;

            template <Component T, typename Self>
            [[nodiscard]]
            auto getComponents(this Self &&self)
                -> std::expected<ConstLike<Self, SparseArray<T>> &,
                                 std::string>;

            template <Component T, typename... Args>
            [[nodiscard]]
            auto addComponent(Entity entity, Args &&...args)
                -> std::expected<T &, std::string>;

            template <Component T, typename Self>
            [[nodiscard]]
            auto view(this Self &&self)
                -> std::span<ConstLike<Self, T>>;

            template <Component T, typename Self>
            [[nodiscard]]
            auto zipView(this Self &&self);

        private:
            std::unordered_map<std::type_index,
                               std::unique_ptr<ISparseArray>> _arrays;

            std::vector<std::uint32_t> _generations; /**< Entity generations */
            std::deque<std::uint32_t> _freeIndices; /**< Recycled entity
                                                    indices */
            std::mutex _mutex; /**< Mutex for thread safety */
    };
}

    #include "Registry.tpp" /* Registry implementation */

#endif /* !RTYPE_REGISTRY_HPP_ */
