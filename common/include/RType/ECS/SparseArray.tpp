/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** SparseArray.tpp
*/

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
    auto &&SparseArray<T>::operator[](this Self &&self, Entity entity) noexcept
        requires (std::is_lvalue_reference_v<Self>)
    {
        RTP_ASSERT(self.has(entity),
                   "SparseArray: Entity {} does not have component " \
                   "(Index out of bounds)", entity.index());

        size_t index = self._sparse[entity.index()];

        return std::forward_like<Self>(self)._data[index];
    }

    template <Component T>
    template <typename... Args>
    auto SparseArray<T>::emplace(Entity entity, Args &&...args) & -> T &
    {
        if (entity.index() >= this->_sparse.size())
            this->_sparse.resize(entity + 1, NullIndex);

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
    auto &&SparseArray<T>::getData(this Self &&self) noexcept
        requires (std::is_lvalue_reference_v<Self>)
    {
        return std::forward<Self>(self)._data;   
    }

    template <Component T>
    template <typename Self>
    auto &&SparseArray<T>::getEntities(this Self &&self) noexcept
        requires (std::is_lvalue_reference_v<Self>)
    {
        return std::forward<Self>(self)._dense;
    }

    template <Component T>
    template <typename Self>
    auto SparseArray<T>::begin(this Self &&self) noexcept
        requires (std::is_lvalue_reference_v<Self>)
    {
        return std::forward<Self>(self)._data.begin();
    }

    template <Component T>
    template <typename Self>
    auto SparseArray<T>::end(this Self &&self) noexcept
        requires (std::is_lvalue_reference_v<Self>)
    {
        return std::forward<Self>(self)._data.end();
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
