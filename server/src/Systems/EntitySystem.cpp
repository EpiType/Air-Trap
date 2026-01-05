/**
 * File   : EntitySystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/EntitySystem.hpp"

namespace rtp::server {
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    EntitySystem::EntitySystem(rtp::ecs::Registry& registry)
        : _registry(registry) {}

    uint32_t EntitySystem::createPlayerEntity(void)
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            throw std::runtime_error(std::string("Failed to spawn player entity: ") + std::string(entityRes.error().message()));
        }

        rtp::ecs::Entity entity = entityRes.value();

        _registry.addComponent<rtp::ecs::components::Transform>(
            entity, 
            rtp::ecs::components::Transform{ {100.f, 100.f}, 0.f, {1.f, 1.f} }
        );

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            entity, 
            rtp::ecs::components::NetworkId{ (uint32_t)entity }
        );

        _registry.addComponent<rtp::ecs::components::server::InputComponent>(
            entity, 
            rtp::ecs::components::server::InputComponent{}
        );

        _registry.addComponent<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ rtp::net::EntityType::Player }
        );

        return entity.index();
    }

    uint32_t EntitySystem::creaetEnemyEntity(const Vec2f& position)
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            throw std::runtime_error(std::string("Failed to spawn enemy entity: ") + std::string(entityRes.error().message()));
        }

        rtp::ecs::Entity entity = entityRes.value();

       _registry.addComponent<rtp::ecs::components::Transform>(
            entity, 
            rtp::ecs::components::Transform{ position, 0.f, {1.f, 1.f} }
        );

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            entity, 
            rtp::ecs::components::NetworkId{ (uint32_t)entity }
        );

        _registry.addComponent<rtp::ecs::components::Velocity>(
            entity, 
            rtp::ecs::components::Velocity{ {-100.f, 0.f} }
        );

        _registry.addComponent<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ rtp::net::EntityType::Scout }
        );

        return entity.index();
    }


} // namespace rtp::server