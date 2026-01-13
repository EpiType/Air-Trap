/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.tpp
*/

#include "Registry.hpp"

namespace rtp::ecs
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    template <Component T, typename Self>
    auto Registry::subscribe(this Self &self)
        -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                          SparseArray<T>>>,
                         rtp::Error>
    {
        std::unique_lock lock(self._mutex);
        std::type_index type = typeid(T);

        if (this->_arrays.contains(type)) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::InternalRuntimeError,
                                                  "Component already registered: {}",
                                                  type.name())};

        return this->_arrays.emplace(type, std::make_unique<SparseArray<T>>());
    }

    template <Component T, typename... Args>
    auto Registry::add(Entity entity, Args &&...args)
        -> std::expected<std::reference_wrapper<T>, rtp::Error>
    {
        auto result = this->get<T>();

        if (!result.has_value()) [[unlikely]]
            return std::unexpected{result.error()};

        std::unique_lock lock(this->_mutex);
        return std::ref(result->get().emplace(entity, std::forward<Args>(args)...));
    }

    template <Component T, typename Self>
    auto Registry::get(this Self &self)
        -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                          SparseArray<T>>>,
                         rtp::Error>
    {
        std::shared_lock lock{self._mutex};
        std::type_index type = typeid(T);

        if (!self._arrays.contains(type)) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::ComponentMissing,
                                                  "Missing component: {}",
                                                  type.name())};

        auto &ptr = self._arrays.find(type)->second;
        auto *rawPtr = static_cast<SparseArray<T> *>(ptr.get());

        return std::ref(*rawPtr);
    }

    template <Component T>
    bool Registry::has(Entity entity) const noexcept
    {
        std::shared_lock lock{this->_mutex};
        std::type_index type = typeid(T);

        if (!this->_arrays.contains(type))
            return false;

        auto &ptr = this->_arrays.find(type)->second;
        auto *rawPtr = static_cast<SparseArray<T> *>(ptr.get());

        return rawPtr->has(entity);
    }

    template <Component T>
    void Registry::remove(Entity entity) noexcept
    {
        std::type_index type = typeid(T);

        {
            std::shared_lock lock{this->_mutex};

            if (!this->_arrays.contains(type)) [[unlikely]]
                return;
        }

        {
            std::unique_lock lock{this->_mutex};
            auto &ptr = this->_arrays.find(type)->second;
            auto *rawPtr = static_cast<SparseArray<T> *>(ptr.get());

            rawPtr->erase(entity);
        }
    }

    template <Component... Ts, typename Self>
    auto Registry::view(this Self &self)
    {
        std::shared_lock lock{self._mutex};
        auto &base = this->getSmallestArray<Ts...>();

        return std::views::iota(std::size_t{0}, base.size())
            | std::views::transform([&](std::size_t i) {
                Entity e = base.entities()[i];
                return std::tuple<Entity, Ts&...>(e, get<Ts>()[e]...);
            })
            | std::views::filter([&](auto const &t) {
                Entity e = std::get<0>(t);
                return (get<Ts>().has(e) && ...);
            });
    }

    template <Component... Ts, typename Self>
    auto Registry::zipView(this Self &self)
    {
        auto get_array = [&]<typename T>() -> ConstLikeRef<Self, SparseArray<T>> {
            std::type_index index(typeid(T));
            
            if (self._arrays.find(index) == self._arrays.end()) {
                throw rtp::Error::failure(ErrorCode::ComponentMissing,
                                          "Component not registered in zipView: {}",
                                          typeid(T).name());
            }

            return static_cast<ConstLikeRef<Self, SparseArray<T>>>(*self._arrays.at(index));
        };

        return ZipView<ConstLikeRef<Self, SparseArray<Ts>>...>(
            get_array.template operator()<Ts>()...);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    template <Component T, typename Self>
    auto Registry::getArray(this Self &self) -> ConstLikeRef<Self, SparseArray<T>>
    {
        std::unique_lock lock{self._mutex};

        auto it = self._arrays.find(typeid(T));
        if (it == self._arrays.end()) {
            auto array = std::make_unique<SparseArray<T>>();
            it = self._arrays.emplace(typeid(T), std::move(array)).first;
        }

        return static_cast<ConstLikeRef<Self, SparseArray<T>>>(*it->second);
    }

    template <Component... Ts>
    auto &Registry::getSmallestArray(void)
    {
        std::array<SparseArray<Ts> *,
                   sizeof...(Ts)> arrays{&getArray<Ts>()...};

        auto it = std::ranges::min_element(arrays, std::less<>(),
                                           [](const auto const *a) { return a->size(); });
        size_t idx = std::distance(arrays.begin(), it);

        return *arrays[idx];
    }

    template <Component... Ts>
    bool Registry::hasAllComponents(Entity entity) const noexcept
    {
        return (... && this->getArray<Ts>(*this).has(entity));
    }
}
