/**
 * File   : ClientNetworkSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "Systems/ClientNetworkSystem.hpp"
#include "RType/Logger.hpp"
#include "RType/ECS/Components/NetworkId.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Animation.hpp"
#include "RType/ECS/Components/EntityType.hpp"
#include "RType/Network/Packet.hpp"
#include "Game/EntityBuilder.hpp"

namespace rtp::client {
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    ClientNetworkSystem::ClientNetworkSystem(ClientNetwork& network, rtp::ecs::Registry& registry)
        : _network(network), _registry(registry) {}

    void ClientNetworkSystem::update(float dt)
    {
        (void)dt;
        while (auto eventOpt = _network.pollEvent()) {
            handleEvent(*eventOpt);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////////////////////////////////

    void ClientNetworkSystem::handleEvent(rtp::net::NetworkEvent& event)
    {
        uint8_t opCodeValue = static_cast<uint8_t>(event.packet.header.opCode);
        switch (opCodeValue)
        {
            case 0x21: { /* EntitySpawn */
                rtp::net::EntitySpawnPayload payload;
                event.packet >> payload;
                spawnEntityFromServer(payload);
                break;
            }
            case 0x22: /* EntityDeath */
                /* code */
                break;
            case 0x20: /* WorldUpdate */
                applyWorldUpdate(event.packet);
                break;
            case 0x03: {/* Disconnect */
                uint32_t entityNetId = 0;
                event.packet >> entityNetId;
                disconnectPlayer(entityNetId);
                break;
            }
        }
    }

    void ClientNetworkSystem::spawnEntityFromServer(const rtp::net::EntitySpawnPayload& payload)
    {
        auto res = _registry.spawnEntity();
        if (!res) return;

        auto e = res.value();

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            e, 
            rtp::ecs::components::NetworkId{payload.netId}
        );
        _registry.addComponent<rtp::ecs::components::Transform>(
            e, 
            rtp::ecs::components::Transform{payload.position, 0.f, {1.f, 1.f}}
        );

        _registry.addComponent<rtp::ecs::components::EntityType>(
            e, 
            rtp::ecs::components::EntityType{static_cast<rtp::net::EntityType>(payload.type)}
        );

        switch (static_cast<rtp::net::EntityType>(payload.type)) {
        case rtp::net::EntityType::Scout:
            addScoutSprite(e);
            break;
        default: 
            break;
        }

        _netIdToEntity[payload.netId] = e;
    }

    void ClientNetworkSystem::addScoutSprite(rtp::ecs::Entity entity)
    {
        rtp::ecs::components::Sprite spriteData;
        spriteData.texturePath = "assets/sprites/r-typesheet2.gif";
        spriteData.rectLeft = 300;
        spriteData.rectTop = 71;
        spriteData.rectWidth = 30;
        spriteData.rectHeight = 18;
        spriteData.zIndex = 5;
        spriteData.red = 255;

        _registry.addComponent<rtp::ecs::components::Sprite>(
            entity, 
            spriteData
        );

        rtp::ecs::components::Animation animData;
        animData.frameLeft = 300;
        animData.frameTop = 71;
        animData.frameWidth = 30;
        animData.frameHeight = 18;
        animData.totalFrames = 6;
        animData.frameDuration = 0.1f;
        animData.currentFrame = 0;
        animData.elapsedTime = 0.0f;

        _registry.addComponent<rtp::ecs::components::Animation>(entity, animData);
    }

    void ClientNetworkSystem::disconnectPlayer(uint32_t entityNetId)
    {
        auto it = _netIdToEntity.find(entityNetId);
        if (it == _netIdToEntity.end()) {
            rtp::log::warning("Disconnect: unknown entityNetId {}", entityNetId);
            return;
        }

        rtp::ecs::Entity e = it->second;

        rtp::log::info("Removing entity netId={} entity={}", entityNetId, (uint32_t)e);

        _registry.killEntity(e);
        _netIdToEntity.erase(it);
    }

    void ClientNetworkSystem::applyWorldUpdate(rtp::net::Packet& packet)
    {
        rtp::net::WorldSnapshotPayload header;
        std::vector<rtp::net::EntitySnapshotPayload> snapshots;

        packet >> header >> snapshots;

        auto view = _registry.zipView<
            rtp::ecs::components::NetworkId,
            rtp::ecs::components::Transform
        >();

        for (const auto& snap : snapshots) {
            bool found = false;
            for (auto&& [netId, tf] : view) {
                if (netId.id == snap.netId) {
                    tf.position.x = snap.position.x;
                    tf.position.y = snap.position.y;
                    found = true;
                    break;
                }
            }
            if (!found) {
                std::cout << "Entity with NetworkId " << snap.netId << " not found in registry.\n";
                spawnEntity(snap);
            }
        }
    }

    void ClientNetworkSystem::spawnEntity(const rtp::net::EntitySnapshotPayload& snap)
    {
        auto entityRes = _registry.spawnEntity();
        
        if (!entityRes) return;
        
        auto entity = entityRes.value();

        rtp::log::info("Spawning Entity NetID: {} at ({}, {})", snap.netId, snap.position.x, snap.position.y);

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            entity, 
            rtp::ecs::components::NetworkId{snap.netId}
        );
                
        _registry.addComponent<rtp::ecs::components::Transform>(
            entity, 
            rtp::ecs::components::Transform{ snap.position, snap.rotation, {1.0f, 1.0f} }
        );

        rtp::ecs::components::Sprite spriteData;
        spriteData.texturePath = "assets/sprites/r-typesheet42.gif";
        spriteData.rectLeft = 0;
        spriteData.rectTop = 0;
        spriteData.rectWidth = 33;
        spriteData.rectHeight = 17;
        spriteData.zIndex = 10;
        spriteData.red = 255;

        _registry.addComponent<rtp::ecs::components::Sprite>(
            entity, 
            spriteData
        );

        rtp::ecs::components::Animation animData;
        animData.frameLeft = 0;
        animData.frameTop = 0;
        animData.frameWidth = 33;
        animData.frameHeight = 17;
        animData.totalFrames = 5;
        animData.frameDuration = 0.1f;
        animData.currentFrame = 0;
        animData.elapsedTime = 0.0f;

        _registry.addComponent<rtp::ecs::components::Animation>(entity, animData);

        _netIdToEntity[snap.netId] = entity;
    }
} // namespace rtp::client