/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SparseArray.tpp
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
 * @file SparseArray.tpp
 * @brief SparseArray template implementations
 * @author Robin Toillon
 * @details Implements the sparse set data structure operations
 * including insertion, removal, lookup, and iteration over components
 * with O(1) access by entity ID.
 */

#include <algorithm>

namespace rtp::ecs
{
    template <Component T>
    void SparseArray<T>::erase(Entity entity) noexcept
    {
        if (!this->has(entity))
            return;

        size_t indexRemoved = this->_sparse[entity.index()];
        size_t indexLast = this->_data.size() - 1;
        Entity entityLast = this->_dense[indexLast];

        if (indexRemoved != indexLast) {
            this->_data[indexRemoved] = std::move(this->_data.back());
            this->_dense[indexRemoved] = entityLast;
            this->_sparse[entityLast.index()] = indexRemoved;
        }

        this->_data.pop_back();
        this->_dense.pop_back();

        this->_sparse[entity.index()] = NullIndex;
    }

    template <Component T>
    bool SparseArray<T>::has(Entity entity) const noexcept
    {
        return entity.index() < this->_sparse.size()
            && this->_sparse[entity.index()] != NullIndex
            && this->_dense[this->_sparse[entity.index()]] == entity;
    }

    template <Component T>
    void SparseArray<T>::clear(void) noexcept
    {
        this->_data.clear();
        this->_dense.clear();
        std::fill(this->_sparse.begin(), this->_sparse.end(), NullIndex);
    }

    template <Component T>
    template <typename Self>
    auto &&SparseArray<T>::operator[](this Self &self, Entity entity) noexcept
    {
        RTP_ASSERT(self.has(entity),
                   "SparseArray: Entity {} does not have component " \
                   "(Index out of bounds)", entity.index());

        size_t index = self._sparse[entity.index()];

        return std::forward_like<Self>(self)._data[index];
    }

    template <Component T>
    template <typename... Args>
    T &SparseArray<T>::emplace(Entity entity, Args &&...args)
    {
        if (entity.index() >= this->_sparse.size())
            this->_sparse.resize(entity.index() + 1, NullIndex);

        size_t index = this->_sparse[entity.index()];
        if (index != NullIndex) {
            this->_data[index] = T(std::forward<Args>(args)...);
            return this->_data[index];
        }

        index = this->_data.size();
        this->_sparse[entity.index()] = index;
        this->_dense.push_back(entity);
        
        return this->_data.emplace_back(std::forward<Args>(args)...);
    }

    template <Component T>
    template <typename Self>
    auto SparseArray<T>::data(this Self &&self) noexcept
    {
        if constexpr (std::is_const_v<std::remove_reference_t<Self>>) {
            return std::span<const T>{self._data};
        } else {
            return std::span<T>{self._data};
        }
    }

    template <Component T>
    template <typename Self>
    auto SparseArray<T>::entities(this Self &&self) noexcept
    {
        if constexpr (std::is_const_v<std::remove_reference_t<Self>>) {
            return std::span<const Entity>{self._dense};
        } else {
            return std::span<Entity>{self._dense};
        }
    }

    template <Component T>
    std::size_t SparseArray<T>::size(void) const noexcept
    {
        return this->_data.size();
    }

    template <Component T>
    bool SparseArray<T>::empty(void) const noexcept
    {
        return this->_data.empty();
    }
}
