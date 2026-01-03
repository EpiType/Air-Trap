/**
 * File   : MovementSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */


#include "Systems/ServerNetworkSystem.hpp"

namespace rtp::server {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    ServerNetworkSystem::ServerNetworkSystem(ServerNetwork& network, rtp::ecs::Registry& registry)
        : _network(network), _registry(registry) {}; 

    void ServerNetworkSystem::update(float dt) 
    {
        (void)dt;
    };

    void ServerNetworkSystem::broadcastWorldState(uint32_t serverTick) {
        std::vector<rtp::net::EntitySnapshotPayload> snapshots;
        
        auto view = _registry.zipView<
            rtp::ecs::components::Transform,
            rtp::ecs::components::NetworkId
        >();

        for (auto&& [tf, net] : view) {
            snapshots.push_back({
                net.id,
                tf.position,
                {0.0f, 0.0f},
                tf.rotation
            });
        }

        if (snapshots.empty()) return;

        rtp::net::Packet packet(rtp::net::OpCode::WorldUpdate);
        rtp::net::WorldSnapshotPayload header = {serverTick, (uint16_t)snapshots.size()};
        
        packet << header << snapshots;
        _network.broadcastPacket(packet, rtp::net::NetworkMode::UDP);
    }

    void ServerNetworkSystem::bindSessionToEntity(uint32_t sessionId, uint32_t entityId) {
        _sessionToEntity[sessionId] = entityId;
    }

    void ServerNetworkSystem::handleInput(uint32_t sessionId, const rtp::net::Packet& packet) {
        rtp::net::InputPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("InputTick: session={} mask={}", sessionId, (int)payload.inputMask);
        if (_sessionToEntity.find(sessionId) == _sessionToEntity.end()) {
            return;
        }
        
        uint32_t entityIndex = _sessionToEntity[sessionId];
        rtp::ecs::Entity entity(entityIndex, 0);

        rtp::ecs::components::server::InputComponent inputData;
        inputData.mask = payload.inputMask;

        _registry.addComponent<rtp::ecs::components::server::InputComponent>(entity, inputData);
    }

    void ServerNetworkSystem::handleDisconnect(uint32_t sessionId) {
        if (_sessionToEntity.find(sessionId) != _sessionToEntity.end()) {
            uint32_t entityIndex = _sessionToEntity[sessionId];
            rtp::ecs::Entity entity(entityIndex, 0);
            _sessionToEntity.erase(sessionId);
            rtp::log::info("Destroyed entity {} for disconnected session {}", entityIndex, sessionId);
        }
    }

    std::pair<int, std::string> ServerNetworkSystem::handlePlayerConnection(uint32_t sessionId, const rtp::net::Packet& packet) {
        rtp::net::PlayerConnectPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("Player connected: session={} username={}", payload.sessionId, payload.username);
        return {sessionId, std::string(payload.username)};
    }

}; // namespace rtp::server