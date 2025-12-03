/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** ComponentArray
*/

#pragma once
#include "RType/ECS/Types.hpp"

namespace rtp::ecs {

class IComponentArray {
   public:
    virtual ~IComponentArray() = default;
    virtual void entityDestroyed(Entity entity) = 0;
};

/**
 * @class ComponentArray
 * @brief Manages a collection of components of a specific type.
 *
 * This class provides methods to insert, remove, and access components
 * associated with entities in an ECS architecture.
 *
 * @tparam T The type of component to be stored in the array.
 */
template <typename Component>
class ComponentArray : public IComponentArray {
   public:
    void insertData(Entity entity, Component component) {
        size_t newIndex = _size;
        _entityToIndexMap[entity] = newIndex;
        _indexToEntityMap[newIndex] = entity;
        _componentArray[newIndex] = component;
        ++_size;
    }

    void removeData(Entity entity) {
        size_t indexOfRemovedEntity = _entityToIndexMap[entity];
        size_t indexOfLastElement = _size - 1;
        _componentArray[indexOfRemovedEntity] = _componentArray[indexOfLastElement];

        Entity entityOfLastElement = _indexToEntityMap[indexOfLastElement];
        _entityToIndexMap[entityOfLastElement] = indexOfRemovedEntity;
        _indexToEntityMap[indexOfRemovedEntity] = entityOfLastElement;

        _entityToIndexMap.erase(entity);
        _indexToEntityMap.erase(indexOfLastElement);

        --_size;
    }

    Component& getData(Entity entity) {
        return _componentArray[_entityToIndexMap[entity]];
    }

    void entityDestroyed(Entity entity) override {
        if (_entityToIndexMap.find(entity) != _entityToIndexMap.end()) {
            removeData(entity);
        }
    }

   private:
    std::array<Component, MAX_ENTITIES> _componentArray;       /**< Array of components */
    std::unordered_map<Entity, std::size_t> _entityToIndexMap; /**< Map from entity to array index */
    std::unordered_map<std::size_t, Entity> _indexToEntityMap; /**< Map from array index to entity */
    size_t _size;                                              /**< Current size of the array */
};
}  // namespace rtp::ecs