/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.tpp
*/

/*
** MIT License
**
** Copyright (c) 2025 Robin Toillon
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * @file Registry.tpp
 * @brief Registry template implementations
 * @author Robin Toillon
 * @details Implements component registration, retrieval, addition, and
 * view creation operations. Provides both single-component views and
 * multi-component zip views for efficient entity iteration.
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
        return *static_cast<SparseArray<T> *>(it->second.get());
    }

    template <Component T, typename Self>
    auto Registry::getComponents(this Self &self)
        -> std::expected<ConstLikeRef<Self, SparseArray<T>>, rtp::Error>
    {
        std::lock_guard lock(self._mutex);
        std::type_index type = typeid(T);

        if (!self._arrays.contains(type)) [[unlikely]]
            return std::unexpected{Error::failure(ErrorCode::ComponentMissing,
                                   "Missing component: {}", type.name())};

        auto &ptr = self._arrays.find(type)->second;
        auto *rawPtr = static_cast<SparseArray<T> *>(ptr.get());

        return std::forward_like<Self>(*rawPtr);
    }

    template <Component T, typename... Args>
    auto Registry::addComponent(Entity entity, Args &&...args) &
        -> std::expected<T &, rtp::Error>
    {
        auto result = this->getComponents<T>();

        if (!result.has_value()) [[unlikely]]
            return std::unexpected{result.error()};

        return result->emplace(entity, std::forward<Args>(args)...);
    }

    template <Component T, typename Self>
    auto Registry::view(this Self &self) -> std::span<ConstLike<Self, T>>
    {
        std::lock_guard lock(self._mutex);
        auto result = self.template getComponents<T>();

        using ComponentType = ConstLike<Self, T>;

        if (!result.has_value()) [[unlikely]] {
            log::warning("Registry::view: {}", result.error());
            return std::span<ComponentType>{};
        }

        return std::span<ComponentType>(result->getData());
    }

    template <Component... Ts, typename Self>
    auto Registry::zipView(this Self &self)
    {
        std::lock_guard lock(self._mutex);
        using FirstComponentType = std::tuple_element_t<0, std::tuple<Ts...>>;

        auto entityResult = self.template getComponents<FirstComponentType>();

        using ViewType = decltype(std::views::zip(
            std::declval<std::span<const Entity>>(),
            std::declval<std::span<ConstLike<Self, Ts>>>()...));

        if (!entityResult.has_value()) [[unlikely]] {
            log::warning("Registry::zipView: {}", entityResult.error());
            return ViewType{};
        }

        auto entitySpan =
            std::span<const Entity>(entityResult.value().getEntities());

        auto componentSpans = std::make_tuple(self.template view<Ts>()...);

        return std::apply(
            [&entitySpan](auto &&...componentViews) {
                return std::views::zip(entitySpan, componentViews...);
            }, componentSpans);
    }
}
