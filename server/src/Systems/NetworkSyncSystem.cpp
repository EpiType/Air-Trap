/**
 * File   : MovementSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */


#include "Systems/NetworkSyncSystem.hpp"
#include "Game/Room.hpp"

namespace rtp::server {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    NetworkSyncSystem::NetworkSyncSystem(ServerNetwork& network, rtp::ecs::Registry& registry)
        : _network(network), _registry(registry) {}; 

    void NetworkSyncSystem::update(float dt) 
    {
        (void)dt;
    };

    void NetworkSyncSystem::bindSessionToEntity(uint32_t sessionId, uint32_t entityId) {
        _sessionToEntity[sessionId] = entityId;
    }

    void NetworkSyncSystem::handleInput(uint32_t sessionId, const rtp::net::Packet& packet) {
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

        _registry.add<rtp::ecs::components::server::InputComponent>(entity, inputData);
    }

    void NetworkSyncSystem::handleDisconnect(uint32_t sessionId) {
        if (_sessionToEntity.find(sessionId) != _sessionToEntity.end()) {
            uint32_t entityIndex = _sessionToEntity[sessionId];
            rtp::ecs::Entity entity(entityIndex, 0);
            _sessionToEntity.erase(sessionId);
            rtp::log::info("Destroyed entity {} for disconnected session {}", entityIndex, sessionId);
        }
    }

    uint32_t NetworkSyncSystem::handlePlayerConnection(uint32_t sessionId, const rtp::net::Packet& packet) {
        rtp::net::ConnectPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("Player new connection [not logged]: session={}", payload.sessionId);
        return {sessionId};
    }

    void NetworkSyncSystem::sendPacketToEntity(uint32_t entityId,
                                           const rtp::net::Packet& packet,
                                           rtp::net::NetworkMode mode)
    {
        for (const auto& [sessionId, eId] : _sessionToEntity) {
            if (eId == entityId) {
                rtp::log::info("sendPacketToEntity: sending packet to session {} for entity {}", sessionId, entityId);
                _network.sendPacket(sessionId, packet, mode);
                return;
            }
        }
        rtp::log::warning("sendPacketToEntity: no session bound to entity {}", entityId);
    }

    void NetworkSyncSystem::sendPacketToSession(uint32_t sessionId,
                                           const rtp::net::Packet& packet,
                                           rtp::net::NetworkMode mode)
    {
        _network.sendPacket(sessionId, packet, mode);
    }

    void NetworkSyncSystem::sendPacketToSessions(const std::vector<uint32_t>& sessions,
                                            const rtp::net::Packet& packet,
                                            rtp::net::NetworkMode mode)
    {
        for (uint32_t sessionId : sessions) {
            _network.sendPacket(sessionId, packet, mode);
        }
    }
}; // namespace rtp::server
