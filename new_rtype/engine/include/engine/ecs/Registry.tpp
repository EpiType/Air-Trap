/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.tpp
*/

#include "engine/ecs/Registry.hpp"

namespace engine::ecs
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    template <Component T, typename Self>
    auto Registry::registerComponent(this Self &self)
        -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                          SparseArray<T>>>,
                         engine::core::Error>
    {
        std::unique_lock lock(self._mutex);
        std::type_index type = typeid(T);

        if (self._arrays.contains(type)) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::InternalRuntimeError,
                                                  "Component already registered: {}",
                                                  type.name())};

        auto [it, inserted] = self._arrays.emplace(
            type, std::make_unique<SparseArray<T>>());
        if (!inserted) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::InternalRuntimeError,
                                                  "Failed to register component: {}",
                                                  type.name())};

        auto *rawPtr = static_cast<ConstLike<Self, SparseArray<T>> *>(it->second.get());
        return std::ref(*rawPtr);
    }

    template <Component T, typename... Args>
    auto Registry::add(Entity entity, Args &&...args)
        -> std::expected<std::reference_wrapper<T>, engine::core::Error>
    {
        auto result = this->get<T>();

        if (!result.has_value()) [[unlikely]]
            return std::unexpected{result.error()};

        std::unique_lock lock(this->_mutex);
        if (entity.index() >= this->_generations.size() ||
            this->_generations[entity.index()] != entity.generation()) {
            return std::unexpected{
                Error::failure(ErrorCode::EntityInvalid,
                               "Registry: Invalid entity")};
        }

        const auto componentId = getStaticComponentID<T>();
        if (componentId >= MAX_COMPONENTS) {
            return std::unexpected{
                Error::failure(ErrorCode::RegistryFull,
                               "Registry: Max component types reached")};
        }

        if (entity.index() >= this->_entitySignatures.size()) {
            this->_entitySignatures.resize(entity.index() + 1);
        }
        this->_entitySignatures[entity.index()].set(componentId);

        return std::ref(result->get().emplace(entity, std::forward<Args>(args)...));
    }

    template <Component T, typename Self>
    auto Registry::get(this Self &self)
        -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                          SparseArray<T>>>,
                         engine::core::Error>
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

    template <Component T, typename Self>
    auto Registry::getComponents(this Self &self)
        -> std::expected<std::reference_wrapper<ConstLike<Self,
                                                          SparseArray<T>>>,
                         engine::core::Error>
    {
        return self.template get<T>();
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
            if (entity.index() < this->_entitySignatures.size()) {
                const auto componentId = getStaticComponentID<T>();
                if (componentId < MAX_COMPONENTS) {
                    this->_entitySignatures[entity.index()].reset(componentId);
                }
            }
        }
    }

    template <Component... Ts, typename Self>
    auto Registry::view(this Self &self)
    {
        if constexpr (sizeof...(Ts) == 1) {
            using T = std::tuple_element_t<0, std::tuple<Ts...>>;
            auto result = self.template get<T>();
            if (!result.has_value()) {
                return std::span<ConstLike<Self, T>>{};
            }
            return result->get().data();
        } else {
            auto &base = self.template getSmallestArray<Ts...>();
            return std::views::iota(std::size_t{0}, base.size())
                | std::views::transform([&](std::size_t i) {
                    Entity e = base.entities()[i];
                    return std::tuple<Entity, Ts&...>(
                        e, self.template getArray<Ts>()[e]...);
                })
                | std::views::filter([&](auto const &t) {
                    Entity e = std::get<0>(t);
                    return (self.template getArray<Ts>().has(e) && ...);
                });
        }
    }

    template <Component... Ts, typename Self>
    auto Registry::zipView(this Self &self)
    {
        auto get_array = [&]<typename T>() -> ConstLikeRef<Self, SparseArray<T>> {
            std::type_index index(typeid(T));
            
            if (self._arrays.find(index) == self._arrays.end()) {
                throw engine::core::Error::failure(ErrorCode::ComponentMissing,
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

    template <Component T, Component... Ts>
    auto &Registry::getSmallestArray(void)
    {
        auto *smallestArray = &this->template getArray<T>();

        (([&]() {
            auto *currentArray = &this->template getArray<Ts>();
            if (currentArray->size() < smallestArray->size()) {
                smallestArray = currentArray;
            }
        }()), ...);

        return *smallestArray;
    }

    template <Component... Ts>
    bool Registry::hasAllComponents(Entity entity) const noexcept
    {
        return (... && this->getArray<Ts>(*this).has(entity));
    }
}
