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

    EntitySystem::EntitySystem(rtp::ecs::Registry& registry, ServerNetwork& network, NetworkSyncSystem& networkSync)
        : _registry(registry), _network(network), _networkSync(networkSync) {}

    void EntitySystem::update(float dt)
    {
        (void)dt;
    }

    rtp::ecs::Entity EntitySystem::createPlayerEntity(PlayerPtr player)
    {
        return createPlayerEntity(player, {100.f, 100.f});
    }

    rtp::ecs::Entity EntitySystem::createPlayerEntity(PlayerPtr player, const rtp::Vec2f& spawnPos)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            rtp::log::error("Failed to spawn player entity: {}", entityRes.error().message());
            throw std::runtime_error(std::string("Failed to spawn player entity: ") + std::string(entityRes.error().message()));
        }

        rtp::ecs::Entity entity = entityRes.value();

        _registry.add<rtp::ecs::components::Transform>(
            entity,
            rtp::ecs::components::Transform{ {spawnPos.x, spawnPos.y}, 0.f, {1.f, 1.f} }
        );

        _registry.add<rtp::ecs::components::Velocity>(
            entity,
            rtp::ecs::components::Velocity{ {0.f, 0.f}, 0.f }
        );

        _registry.add<rtp::ecs::components::SimpleWeapon>(
            entity,
            rtp::ecs::components::SimpleWeapon{ 6.0f, 0.0f, 10 }
        );

        _registry.add<rtp::ecs::components::Ammo>(
            entity,
            rtp::ecs::components::Ammo{ 100, 100, 2.0f, 0.0f, false, true }
        );

        _registry.add<rtp::ecs::components::MovementSpeed>(
            entity,
            rtp::ecs::components::MovementSpeed{ 200.0f, 1.0f, 0.0f }
        );

        _registry.add<rtp::ecs::components::NetworkId>(
            entity,
            rtp::ecs::components::NetworkId{ (uint32_t)entity }
        );

        _registry.add<rtp::ecs::components::server::InputComponent>(
            entity, 
            rtp::ecs::components::server::InputComponent{}
        );

        _registry.add<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ rtp::net::EntityType::Player }
        );

        _registry.add<rtp::ecs::components::Health>(
            entity,
            rtp::ecs::components::Health{ 100, 100 }
        );

        _registry.add<rtp::ecs::components::BoundingBox>(
            entity,
            rtp::ecs::components::BoundingBox{ 32.0f, 16.0f }
        );

        _registry.add<rtp::ecs::components::RoomId>(
            entity,
            rtp::ecs::components::RoomId{ player->getRoomId() }
        );
        
        return entity;
    }

    rtp::ecs::Entity EntitySystem::createEnemyEntity(
        uint32_t roomId,
        const rtp::Vec2f& pos,
        rtp::ecs::components::Patterns pattern,
        float speed,
        float amplitude,
        float frequency,
        rtp::net::EntityType type
    )
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            rtp::log::error("Failed to spawn enemy entity: {}", entityRes.error().message());
            throw std::runtime_error(std::string("Failed to spawn enemy entity: ") + std::string(entityRes.error().message()));
        }

        rtp::ecs::Entity entity = entityRes.value();

        _registry.add<rtp::ecs::components::Transform>(
            entity,
            rtp::ecs::components::Transform{ {pos.x, pos.y}, 0.f, {1.f, 1.f} }
        );

        _registry.add<rtp::ecs::components::NetworkId>(
            entity,
            rtp::ecs::components::NetworkId{ static_cast<uint32_t>(entity.index()) }
        );

        _registry.add<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ type }
        );

        _registry.add<rtp::ecs::components::RoomId>(
            entity,
            rtp::ecs::components::RoomId{ roomId }
        );

        int maxHealth = 40;
        if (type == rtp::net::EntityType::Tank) {
            maxHealth = 80;
        } else if (type == rtp::net::EntityType::Boss) {
            maxHealth = 200;
        }
        _registry.add<rtp::ecs::components::Health>(
            entity,
            rtp::ecs::components::Health{ maxHealth, maxHealth }
        );

        _registry.add<rtp::ecs::components::Velocity>(
            entity,
            rtp::ecs::components::Velocity{ {0.f, 0.f}, 0.f }
        );

        _registry.add<rtp::ecs::components::MouvementPattern>(
            entity,
            rtp::ecs::components::MouvementPattern{ pattern, speed, amplitude, frequency }
        );

        float fireRate = 0.6f;
        if (type == rtp::net::EntityType::Tank) {
            fireRate = 0.4f;
        } else if (type == rtp::net::EntityType::Boss) {
            fireRate = 1.2f;
        }
        _registry.add<rtp::ecs::components::SimpleWeapon>(
            entity,
            rtp::ecs::components::SimpleWeapon{ fireRate, 0.0f, 0 }
        );

        _registry.add<rtp::ecs::components::BoundingBox>(
            entity,
            rtp::ecs::components::BoundingBox{ 30.0f, 18.0f }
        );

        // _registry.add<rtp::ecs::components::IABehaviorComponent>(
        //     entity,
        //     rtp::ecs::components::IABehaviorComponent{ rtp::ecs::components::IABehavior::Passive, 500.0 }
        // );

        return entity;
    }

    rtp::ecs::Entity EntitySystem::creaetEnemyEntity(
        uint32_t roomId,
        const rtp::Vec2f& pos,
        rtp::ecs::components::Patterns pattern,
        float speed,
        float amplitude,
        float frequency
    )
    {
        return createEnemyEntity(roomId, pos, pattern, speed, amplitude, frequency, rtp::net::EntityType::Scout);
    }

    rtp::ecs::Entity EntitySystem::createPowerupEntity(
        uint32_t roomId,
        const rtp::Vec2f& pos,
        rtp::ecs::components::PowerupType type,
        float value,
        float duration
    )
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            rtp::log::error("Failed to spawn powerup entity: {}", entityRes.error().message());
            throw std::runtime_error(std::string("Failed to spawn powerup entity: ") + std::string(entityRes.error().message()));
        }

        rtp::ecs::Entity entity = entityRes.value();

        _registry.add<rtp::ecs::components::Transform>(
            entity,
            rtp::ecs::components::Transform{ {pos.x, pos.y}, 0.f, {1.f, 1.f} }
        );

        const rtp::net::EntityType netType = (type == rtp::ecs::components::PowerupType::Speed)
            ? rtp::net::EntityType::PowerupSpeed
            : rtp::net::EntityType::PowerupHeal;

        _registry.add<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ netType }
        );

        _registry.add<rtp::ecs::components::Powerup>(
            entity,
            rtp::ecs::components::Powerup{ type, value, duration }
        );

        _registry.add<rtp::ecs::components::BoundingBox>(
            entity,
            rtp::ecs::components::BoundingBox{ 16.0f, 16.0f }
        );

        _registry.add<rtp::ecs::components::NetworkId>(
            entity,
            rtp::ecs::components::NetworkId{ static_cast<uint32_t>(entity.index()) }
        );

        _registry.add<rtp::ecs::components::RoomId>(
            entity,
            rtp::ecs::components::RoomId{ roomId }
        );

        return entity;
    }

    rtp::ecs::Entity EntitySystem::createObstacleEntity(
        uint32_t roomId,
        const rtp::Vec2f& pos,
        const rtp::Vec2f& size,
        int health,
        rtp::net::EntityType type
    )
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            rtp::log::error("Failed to spawn obstacle entity: {}", entityRes.error().message());
            throw std::runtime_error(std::string("Failed to spawn obstacle entity: ") + std::string(entityRes.error().message()));
        }

        rtp::ecs::Entity entity = entityRes.value();

        _registry.add<rtp::ecs::components::Transform>(
            entity,
            rtp::ecs::components::Transform{ {pos.x, pos.y}, 0.f, {1.f, 1.f} }
        );

        _registry.add<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ type }
        );

        _registry.add<rtp::ecs::components::Health>(
            entity,
            rtp::ecs::components::Health{ health, health }
        );

        _registry.add<rtp::ecs::components::BoundingBox>(
            entity,
            rtp::ecs::components::BoundingBox{ size.x, size.y }
        );

        _registry.add<rtp::ecs::components::NetworkId>(
            entity,
            rtp::ecs::components::NetworkId{ static_cast<uint32_t>(entity.index()) }
        );

        _registry.add<rtp::ecs::components::RoomId>(
            entity,
            rtp::ecs::components::RoomId{ roomId }
        );

        return entity;
    }
} // namespace rtp::server
