/**
 * File   : RenderSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_ECS_ZIPVIEW_HPP_
#define RTYPE_ECS_ZIPVIEW_HPP_

#include "RType/ECS/SparseArray.hpp"
#include <tuple>
#include <vector>
#include <algorithm>
#include <iterator>

namespace rtp::ecs {

    /**
     * @class ZipView
     * @brief A view that iterates over entities possessing all specified components.
     * @tparam Containers The types of the SparseArrays (e.g., SparseArray<Pos>&, SparseArray<Vel>&)
     */
    template <class... Containers>
    class ZipView {
    public:
        /**
         * @brief Tuple type holding references to all SparseArrays
         */
        using tuple_arrays_t = std::tuple<Containers...>;

        /**
         * @class Iterator
         * @brief The actual iterator performing the intersection logic
         */
        class Iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = std::tuple<typename std::remove_reference_t<Containers>::value_type&...>;
            using difference_type = std::ptrdiff_t;
            using pointer = void;
            using reference = value_type;

            Iterator(tuple_arrays_t& arrays, 
                     const std::vector<Entity>* entities, 
                     size_t index)
                : _arrays(arrays), _entities(entities), _index(index)
            {
                skipInvalid();
            }

            Iterator& operator++() {
                ++_index;
                skipInvalid();
                return *this;
            }

            Iterator operator++(int) {
                Iterator tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const Iterator& other) const {
                return _index == other._index;
            }

            bool operator!=(const Iterator& other) const {
                return _index != other._index;
            }

            [[nodiscard]]
            reference operator*() {
                Entity e = (*_entities)[_index];
                return std::apply([e](auto&&... args) {
                    return std::forward_as_tuple(args[e]...);
                }, _arrays);
            }

        private:
            tuple_arrays_t& _arrays;                /**< Tuple of references to SparseArrays */
            const std::vector<Entity>* _entities;   /**< Pointer to the entity list of the leader SparseArray */
            size_t _index;                          /**< Current index in the entity list */

            /** 
             * @brief Skip to the next valid entity that has all components
             */
            void skipInvalid() {
                while (_index < _entities->size()) {
                    Entity e = (*_entities)[_index];
                    
                    bool all_present = std::apply([e](auto&&... args) {
                        return (... && args.has(e)); 
                    }, _arrays);

                    if (all_present) {
                        return;
                    }
                    
                    ++_index;
                }
            }
        };

        /**
         * @brief Constructor for ZipView
         * @param arrays References to the SparseArrays to zip
         */
        ZipView(Containers... arrays) : _arrays(std::forward_as_tuple(arrays...)) {
            size_t min_size = std::get<0>(_arrays).size();
            _smallest_idx = 0;
            findSmallest<0>(min_size);
        }

        /**
         * @brief Get iterator to the beginning of the zipped view
         * @return Iterator to the first valid entity with all components
         */
        Iterator begin() {
            const auto& entities = getEntitiesFromSmallest(_smallest_idx);
            return Iterator(_arrays, &entities, 0);
        }

        /**
         * @brief Get iterator to the end of the zipped view
         * @return Iterator past the last entity
         */
        Iterator end() {
            const auto& entities = getEntitiesFromSmallest(_smallest_idx);
            return Iterator(_arrays, &entities, entities.size());
        }

    private:
        tuple_arrays_t _arrays;          /**< Tuple of references to SparseArrays */
        size_t _smallest_idx = 0;        /**< Index of the SparseArray with the fewest entities */

        /** 
         * @brief Find the index of the SparseArray with the smallest size
         * @tparam I Current index in the tuple
         * @param min_size Reference to the current minimum size found
         */
        template<size_t I>
        void findSmallest(size_t& min_size) {
            if constexpr (I < sizeof...(Containers)) {
                size_t current_size = std::get<I>(_arrays).size();
                if (current_size < min_size) {
                    min_size = current_size;
                    _smallest_idx = I;
                }
                findSmallest<I + 1>(min_size);
            }
        }

        /** 
         * @brief Get the entity list from the SparseArray at the specified index
         * @param index Index of the SparseArray in the tuple
         * @return Reference to the vector of entities
         */
        const std::vector<Entity>& getEntitiesFromSmallest(size_t index) {
            const std::vector<Entity>* result = nullptr;
            size_t current = 0;
            
            std::apply([&](auto&&... args) {
                ((current++ == index ? result = &args.getEntities() : nullptr), ...);
            }, _arrays);
            
            return *result;
        }
    };
}

#endif // RTYPE_ECS_ZIPVIEW_HPP_