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
        std::type_index type = typeid(T);

        if (this->_arrays.contains(type)) {
            return *static_cast<SparseArray<T> *>(
                this->_arrays.find(type)->second.get());
        }

        auto sparseArray = std::make_unique<SparseArray<T>>();
        auto *ptr = sparseArray.get();

        this->_arrays.emplace(type, std::move(sparseArray));

        return *ptr;
    }

    template <Component T, typename Self>
    auto Registry::getComponents(this Self &&self)
        -> std::expected<ConstLike<Self, SparseArray<T>> &, std::string>
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
    auto Registry::addComponent(Entity entity, Args &&...args)
        -> std::expected<T &, std::string>
    {
        auto result = this->getComponents<T>();

        if (!result.has_value()) [[unlikely]]
            return std::unexpected(result.error());

        return result->emplace(entity, std::forward<Args>(args)...);
    }

    template <Component T, typename Self>
    auto Registry::view(this Self &&self)
        -> std::span<ConstLike<Self, T>>
    {
        auto result = self.template getComponents<T>();

        using ComponentType = ConstLike<Self, T>;

        if (!result.has_value()) [[unlikely]] {
            log::warning("Registry::view: {}", result.error());
            return std::span<ComponentType>{};
        }

        return std::span<ComponentType>(result->getData());
    }

    template <Component T, typename Self>
    auto Registry::zipView(this Self &&self)
    {
        auto result = self.template getComponents<T>();

        using ComponentType = ConstLike<Self, T>;

        if (!result.has_value()) [[unlikely]] {
            log::warning("Registry::zip_view: {}", result.error());
            return std::views::zip(std::span<const Entity>{},
                                   std::span<ComponentType>{});
        }

        auto &components = result.value();

        return std::views::zip(
            std::span<const Entity>(components.getEntities()), 
            std::span<ComponentType>(components.getData()));
    }
}
