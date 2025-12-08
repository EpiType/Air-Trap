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
            std::swap(this->_data[indexRemoved],
                      this->_data[indexLast]);

            std::swap(this->_dense[indexRemoved],
                      this->_dense[indexLast]);

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
    {
        RTP_ASSERT(self.has(entity),
                   "SparseArray: Entity {} does not have component " \
                   "(Index out of bounds)", entity.index());

        size_t index = self._sparse[entity.index()];

        return std::forward<Self>(self)._data[index];
    }

    template <Component T>
    template <typename... Args>
    auto SparseArray<T>::emplace(Entity entity, Args &&...args) -> T &
    {
        if (entity >= this->_sparse.size())
            this->_sparse.resize(entity + 1, NullIndex);

        if (this->_sparse[entity.index()] != NullIndex) {
            size_t index = this->_sparse[entity.index()];
            this->_data[index] = T(std::forward<Args>(args)...);
            return this->_data[index];
        }

        size_t index = this->_data.size();
        this->_sparse[entity] = index;
        this->_dense.push_back(entity);
        
        return this->_data.emplace_back(std::forward<Args>(args)...);
    }

    template <Component T>
    template <typename Self>
    auto &&SparseArray<T>::getData(this Self &&self) noexcept
    {
        return std::forward<Self>(self)._data;   
    }

    template <Component T>
    template <typename Self>
    auto &&SparseArray<T>::getEntities(this Self &&self) noexcept
    {
        return std::forward<Self>(self)._dense;
    }

    template <Component T>
    template <typename Self>
    auto SparseArray<T>::begin(this Self &&self) noexcept
    {
        return std::forward<Self>(self)._data.begin();
    }

    template <Component T>
    template <typename Self>
    auto SparseArray<T>::end(this Self &&self) noexcept
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
