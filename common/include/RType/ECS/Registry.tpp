/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.tpp
*/

#include "RType/ECS/ZipView.hpp"
#include <functional> // std::ref

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
        return *static_cast<SparseArray<T> *>(it->second.get());
    }

    template <Component T, typename Self>
    auto Registry::getComponents(this Self &self)
        -> std::expected<std::reference_wrapper<ConstLike<Self, SparseArray<T>>>, rtp::Error>
    {
        std::lock_guard lock(self._mutex);
        std::type_index type = typeid(T);

        if (!self._arrays.contains(type)) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::ComponentMissing, "Missing component: {}", type.name())};

        auto &ptr = self._arrays.find(type)->second;
        auto *rawPtr = static_cast<SparseArray<T> *>(ptr.get());

        return std::ref(*rawPtr);
    }

    template <Component T, typename... Args>
    auto Registry::addComponent(Entity entity, Args &&...args)
        -> std::expected<std::reference_wrapper<T>, rtp::Error>
    {
        auto result = this->getComponents<T>();

        if (!result.has_value()) [[unlikely]]
            return std::unexpected{result.error()};

        return std::ref(result->get().emplace(entity, std::forward<Args>(args)...));
    }

    template <Component T, typename Self>
    auto Registry::view(this Self &self) -> std::span<ConstLike<Self, T>>
    {
        std::lock_guard lock(self._mutex);
        auto result = self.template getComponents<T>();

        using ComponentType = ConstLike<Self, T>;

        if (!result.has_value()) [[unlikely]] {
            return std::span<ComponentType>{};
        }

        return std::span<ComponentType>(result->get().getData());
    }

    template <Component... Ts, typename Self>
    auto Registry::zipView(this Self &self)
    {
        auto get_array = [&]<typename T>() -> ConstLikeRef<Self, SparseArray<T>> {
            std::type_index index(typeid(T));
            
            if (self._arrays.find(index) == self._arrays.end()) {
                throw rtp::Error::failure(ErrorCode::ComponentMissing, "Component not registered in zipView: {}", typeid(T).name());
            }

            return static_cast<ConstLikeRef<Self, SparseArray<T>>>(*self._arrays.at(index));
        };

        return ZipView<ConstLikeRef<Self, SparseArray<Ts>>...>(
            get_array.template operator()<Ts>()...
        );
    }
}