#pragma once

#include "Entity.hpp"
#include <memory>
#include <unordered_map>
#include <vector>
#include <typeindex>
#include <any>

namespace ecs {

/**
 * @brief The Registry manages entities and their components.
 * 
 * This is a simplified ECS implementation. For production,
 * consider using a library like EnTT.
 */
class Registry {
public:
    Registry() = default;
    ~Registry() = default;

    // Entity management
    Entity createEntity();
    void destroyEntity(Entity entity);

    // Component management
    template<typename T>
    T& addComponent(Entity entity, T component);

    template<typename T>
    T& getComponent(Entity entity);

    template<typename T>
    bool hasComponent(Entity entity) const;

    template<typename T>
    void removeComponent(Entity entity);

    // View for systems to iterate over entities with specific components
    template<typename... Components>
    std::vector<Entity> view();

private:
    Entity _nextEntityId = 1;
    std::vector<Entity> _entities;
    
    // Component storage: TypeIndex -> (Entity -> Component)
    std::unordered_map<std::type_index, std::unordered_map<Entity, std::any>> _components;
};

// Template implementations would go in Registry.inl or be included here

} // namespace ecs
