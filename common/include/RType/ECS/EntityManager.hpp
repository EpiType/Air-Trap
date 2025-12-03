/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** EntityManager
*/

#pragma once
#include <queue>

#include "RType/ECS/Component.hpp"
#include "RType/ECS/Types.hpp"

namespace rtp::ecs {
/**
 * @class EntityManager
 * @brief Manages the creation, destruction, and tracking of entities.
 *
 * The EntityManager is responsible for generating unique entity IDs,
 * destroying entities, and maintaining the signatures associated with each entity.
 */
class EntityManager {
   public:
    EntityManager() {
        for (Entity entity = 0; entity < MAX_ENTITIES; ++entity) {
            _availableEntities.push(entity);
        }
    };
    ~EntityManager() = default;

    Entity createEntity(void) {
        Entity id = _availableEntities.front();
        _availableEntities.pop();
        return id;
    }

    void destroyEntity(Entity entity) {
        mSignatures[entity].reset();
        _availableEntities.push(entity);
    }

    void setSignature(Entity entity, Signature signature) {
        mSignatures[entity] = signature;
    }

    Signature getSignature(Entity entity) {
        return mSignatures[entity];
    }

   private:
    std::queue<Entity> _availableEntities;
    std::array<Signature, MAX_ENTITIES> mSignatures{};
};
}  // namespace rtp::ecs