/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.tpp
*/

namespace rtp::ecs
{
    template <Component T>
    auto Registry::registerComponent(void) -> SparseArray<T> &
    {
        std::lock_guard lock(this->_mutex);
        std::type_index type = typeid(T);

        if (this->_arrays.contains(type)) {
            return *static_cast<SparseArray<T> *>(
                this->_arrays.find(type)->second.get());
        }

        auto [it, ok] =
            this->_arrays.emplace(type, std::make_unique<SparseArray<T>>());
        return *static_cast<SparseArray<T>*>(it->second.get());
    }

    template <Component T, typename Self>
    auto Registry::getComponents(this Self &&self)
        -> std::expected<ConstLikeRef<Self, SparseArray<T>>, std::string>
        requires (std::is_lvalue_reference_v<Self>)
    {
        std::type_index type = typeid(T);

        if (!self._arrays.contains(type)) [[unlikely]]
            return std::unexpected(std::format("Missing component: {}",
                                               type.name()));

        auto &ptr = self._arrays.find(type)->second;
        auto *rawPtr = static_cast<SparseArray<T> *>(ptr.get());

        return std::forward_like<Self>(*rawPtr);
    }

    template <Component T, typename... Args>
    auto Registry::addComponent(Entity entity, Args &&...args) &
        -> std::expected<T &, std::string>
    {
        auto result = this->getComponents<T>();

        if (!result.has_value()) [[unlikely]]
            return std::unexpected(result.error());

        return result->emplace(entity, std::forward<Args>(args)...);
    }

    template <Component T, typename Self>
    auto Registry::view(this Self &&self) -> std::span<ConstLike<Self, T>>
        requires (std::is_lvalue_reference_v<Self>)
    {
        auto result = self.template getComponents<T>();

        using ComponentType = ConstLike<Self, T>;

        if (!result.has_value()) [[unlikely]] {
            log::warning("Registry::view: {}", result.error());
            return std::span<ComponentType>{};
        }

        return std::span<ComponentType>(result->getData());
    }

    template <Component ...Ts, typename Self>
    auto Registry::zipView(this Self &&self)
        requires (std::is_lvalue_reference_v<Self>)
    {
        using FirstComponentType = std::tuple_element_t<0, std::tuple<Ts...>>;
    
        auto entityResult = self.template getComponents<FirstComponentType>();

        if (!entityResult.has_value())
            return std::views::zip();

        auto entitySpan = std::span<const Entity>(
                              entityResult.value().getEntities());

        auto componentSpans = std::make_tuple(self.template view<Ts>()...);

        return std::apply([&entitySpan](auto &&...componentViews) {
            return std::views::zip(entitySpan, componentViews...);
        }, componentSpans);
    }
}
