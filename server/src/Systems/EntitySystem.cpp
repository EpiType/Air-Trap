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

    EntitySystem::EntitySystem(rtp::ecs::Registry& registry, ServerNetwork& network)
        : _registry(registry), _network(network) {}

    void EntitySystem::update(float dt)
    {
        (void)dt;
    }

    uint32_t EntitySystem::createPlayerEntity(PlayerPtr player)
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            rtp::log::error("Failed to spawn player entity: {}", entityRes.error().message());
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

        player->setEntityId(entity.index());

        rtp::log::info("Spawned Entity {} for Player {}", entity.index(), player->getId());
        rtp::net::Packet packet(rtp::net::OpCode::EntitySpawn);
        rtp::net::EntitySpawnPayload payload = {
            static_cast<uint32_t>(entity.index()),
            (uint8_t)rtp::net::EntityType::Player,
            100.f,
            100.f
        };
        packet << payload;
        _network.broadcastPacket(packet, rtp::net::NetworkMode::TCP);

        return static_cast<uint32_t>(entity.index());
    }

    uint32_t EntitySystem::creaetEnemyEntity(
        uint32_t roomId,
        const rtp::Vec2f& pos,
        rtp::ecs::components::Patterns pattern = rtp::ecs::components::Patterns::StraightLine,
        float speed = 120.0f,
        float amplitude = 40.0f,
        float frequency = 2.0f
    )
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            rtp::log::error("Failed to spawn enemy entity: {}", entityRes.error().message());
            throw std::runtime_error(std::string("Failed to spawn enemy entity: ") + std::string(entityRes.error().message()));
        }

        rtp::ecs::Entity entity = entityRes.value();

        _registry.addComponent<rtp::ecs::components::Transform>(
            entity,
            rtp::ecs::components::Transform{ {pos.x, pos.y}, 0.f, {1.f, 1.f} }
        );

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            entity,
            rtp::ecs::components::NetworkId{ static_cast<uint32_t>(entity.index()) }
        );

        _registry.addComponent<rtp::ecs::components::EntityType>(
            entity,
            rtp::ecs::components::EntityType{ rtp::net::EntityType::Scout }
        );

        _registry.addComponent<rtp::ecs::components::RoomId>(
            entity,
            rtp::ecs::components::RoomId{ roomId }
        );

        _registry.addComponent<rtp::ecs::components::MouvementPattern>(
            entity,
            rtp::ecs::components::MouvementPattern{ pattern, speed, amplitude, frequency }
        );

        // _registry.addComponent<rtp::ecs::components::IABehaviorComponent>(
        //     entity,
        //     rtp::ecs::components::IABehaviorComponent{ rtp::ecs::components::IABehavior::Passive, 500.0 }
        // );

        rtp::log::info("Spawned Enemy Entity {} in room {}", entity.index(), roomId);
        rtp::net::Packet packet(rtp::net::OpCode::EntitySpawn);
        rtp::net::EntitySpawnPayload payload = {
            static_cast<uint32_t>(entity.index()),
            (uint8_t)rtp::net::EntityType::Scout,
            pos.x,
            pos.y
        };
        packet << payload;
        _network.broadcastPacket(packet, rtp::net::NetworkMode::TCP);

        return static_cast<uint32_t>(entity.index());
    }
} // namespace rtp::server