/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** ShieldSystem implementation
*/

#include "Systems/ShieldSystem.hpp"
#include "RType/ECS/Components/ShieldVisual.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/BoundingBox.hpp"
#include "RType/ECS/Components/EntityType.hpp"
#include "RType/ECS/Components/Controllable.hpp"
#include "RType/Logger.hpp"

#include <cmath>

namespace rtp::client {

ShieldSystem::ShieldSystem(ecs::Registry& registry)
    : _registry(registry)
{
}

void ShieldSystem::update(float dt)
{
    auto shieldVisualsOpt = _registry.get<rtp::ecs::components::ShieldVisual>();
    if (!shieldVisualsOpt) return;
    
    auto& shieldVisuals = shieldVisualsOpt.value().get();
    
    for (auto entity : shieldVisuals.entities()) {
        if (!shieldVisuals.has(entity)) continue;
        
        auto& shield = shieldVisuals[entity];
        shield.animationTime += dt;
        
        const float pulse = std::sin(shield.animationTime * 3.0f) * 0.5f + 0.5f;
        shield.alpha = 150.0f + pulse * 105.0f;
    }
    
    auto playerTransformsOpt = _registry.get<ecs::components::Transform>();
    auto playerBoxesOpt = _registry.get<ecs::components::BoundingBox>();
    auto playerTypesOpt = _registry.get<ecs::components::EntityType>();
    auto bulletTransformsOpt = _registry.get<ecs::components::Transform>();
    auto bulletBoxesOpt = _registry.get<ecs::components::BoundingBox>();
    auto entityTypesOpt = _registry.get<ecs::components::EntityType>();
    
    if (!playerTransformsOpt || !playerBoxesOpt || !playerTypesOpt ||
        !bulletTransformsOpt || !bulletBoxesOpt || !entityTypesOpt) {
        return;
    }
    
    auto& playerTransforms = playerTransformsOpt.value().get();
    auto& playerBoxes = playerBoxesOpt.value().get();
    auto& playerTypes = playerTypesOpt.value().get();
    auto& bulletTransforms = bulletTransformsOpt.value().get();
    auto& bulletBoxes = bulletBoxesOpt.value().get();
    auto& entityTypes = entityTypesOpt.value().get();
    
    std::vector<ecs::Entity> shieldsToRemove;
    std::vector<ecs::Entity> bulletsToDestroy;
    
    for (auto playerEntity : shieldVisuals.entities()) {
        if (!shieldVisuals.has(playerEntity)) continue;
        if (!playerTypes.has(playerEntity)) continue;
        if (playerTypes[playerEntity].type != net::EntityType::Player) continue;
        
        if (!playerTransforms.has(playerEntity)) continue;
        if (!playerBoxes.has(playerEntity)) continue;
        
        const auto& playerTrans = playerTransforms[playerEntity];
        const auto& playerBox = playerBoxes[playerEntity];
        
        for (auto bulletEntity : entityTypes.entities()) {
            if (!entityTypes.has(bulletEntity)) continue;
            if (entityTypes[bulletEntity].type != net::EntityType::EnemyBullet) continue;
            if (!bulletTransforms.has(bulletEntity)) continue;
            if (!bulletBoxes.has(bulletEntity)) continue;
            
            const auto& bulletTrans = bulletTransforms[bulletEntity];
            const auto& bulletBox = bulletBoxes[bulletEntity];
            
            const bool collisionX = playerTrans.position.x < bulletTrans.position.x + bulletBox.width &&
                                    playerTrans.position.x + playerBox.width > bulletTrans.position.x;
            
            const bool collisionY = playerTrans.position.y < bulletTrans.position.y + bulletBox.height &&
                                    playerTrans.position.y + playerBox.height > bulletTrans.position.y;
            
            if (collisionX && collisionY) {
                shieldsToRemove.push_back(playerEntity);
                bulletsToDestroy.push_back(bulletEntity);
                log::info("🛡️ Shield absorbed bullet!");
                break;
            }
        }
    }
    

    for (const auto& entity : shieldsToRemove) {
        _registry.remove<rtp::ecs::components::ShieldVisual>(entity);
    }
    
    for (const auto& entity : bulletsToDestroy) {
        _registry.kill(entity);
    }
}

} // namespace rtp::client
