/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** ComponentManager
*/

#pragma once
#include <cassert>
#include <memory>
#include <unordered_map>

#include "RType/ECS/Types.hpp"
#include "RType/ECS/ComponentArray.hpp"

namespace rtp::ecs {
class ComponentManager {
   public:
    template <typename T>
    void registerComponent(void) {
        const char* typeName = typeid(T).name();
        assert(mComponentTypes.find(typeName) == mComponentTypes.end() && "Registering component type more than once.");
        mComponentTypes.insert({typeName, mNextComponentType});
        mComponentArrays.insert({typeName, std::make_shared<ComponentArray<T>>()});
        ++mNextComponentType;
    }

    template <typename T>
    Component getComponentType(void) {
        const char* typeName = typeid(T).name();
        assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");
        return mComponentTypes[typeName];
    }

    template <typename T>
    void addComponent(Entity entity, T component) {
        GetComponentArray<T>()->insertData(entity, component);
    }

    template <typename T>
    void removeComponent(Entity entity) {
        GetComponentArray<T>()->removeData(entity);
    }

    template <typename T>
    T& getComponent(Entity entity) {
        return GetComponentArray<T>()->getData(entity);
    }

    void entityDestroyed(Entity entity) {
        for (auto const& pair : mComponentArrays) {
            auto const& component = pair.second;
            component->entityDestroyed(entity);
        }
    }

   private:
    std::unordered_map<const char*, Component> mComponentTypes{};

    std::unordered_map<const char*, std::shared_ptr<IComponentArray>> mComponentArrays{};

    Component mNextComponentType{};

    template <typename T>
    std::shared_ptr<ComponentArray<T>> GetComponentArray() {
        const char* typeName = typeid(T).name();

        assert(mComponentTypes.find(typeName) != mComponentTypes.end() && "Component not registered before use.");

        return std::static_pointer_cast<ComponentArray<T>>(mComponentArrays[typeName]);
    }
};
}  // namespace rtp::ecs