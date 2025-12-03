/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Coordinator
*/

#pragma once
#include <memory>

#include "RType/ECS/ComponentManager.hpp"
#include "RType/ECS/EntityManager.hpp"
#include "RType/ECS/System/SystemManager.hpp"

namespace rtp::ecs {
class Coordinator {
   public:
    void init() {
        _ComponentManager = std::make_unique<ComponentManager>();
        _EntityManager = std::make_unique<EntityManager>();
        _SystemManager = std::make_unique<SystemManager>();
    }

    Entity createEntity() {
        return _EntityManager->createEntity();
    }

    void destroyEntity(Entity entity) {
        _EntityManager->destroyEntity(entity);
        _ComponentManager->entityDestroyed(entity);
        _SystemManager->entityDestroyed(entity);
    }

    template <typename T>
    void registerComponent() {
        _ComponentManager->RegisterComponent<T>();
    }

    template <typename T>
    void addComponent(Entity entity, T component) {
        mComponentManager->AddComponent<T>(entity, component);

        auto signature = _EntityManager->GetSignature(entity);
        signature.set(_ComponentManager->GetComponentType<T>(), true);
        _EntityManager->SetSignature(entity, signature);

        _SystemManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T>
    void removeComponent(Entity entity) {
        mComponentManager->RemoveComponent<T>(entity);

        auto signature = _EntityManager->GetSignature(entity);
        signature.set(_ComponentManager->GetComponentType<T>(), false);
        _EntityManager->SetSignature(entity, signature);

        _SystemManager->EntitySignatureChanged(entity, signature);
    }

    template <typename T>
    T& getComponent(Entity entity) {
        return _ComponentManager->GetComponent<T>(entity);
    }

    template <typename T>
    Component getComponentType() {
        return _ComponentManager->GetComponentType<T>();
    }

    template <typename T>
    std::shared_ptr<T> registerSystem() {
        return _SystemManager->RegisterSystem<T>();
    }

    template <typename T>
    void setSystemSignature(Signature signature) {
        _SystemManager->SetSignature<T>(signature);
    }

   private:
    std::unique_ptr<ComponentManager> _ComponentManager;
    std::unique_ptr<EntityManager> _EntityManager;
    std::unique_ptr<SystemManager> _SystemManager;
};
}  // namespace rtp::ecs