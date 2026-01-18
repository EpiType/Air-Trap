/**
 * File   : MovementSystem.hpp
 * License: MIT
 * Author : Elias Josué
 * HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   :
 * 11/12/2025
 */

#include "Systems/NetworkSyncSystem.hpp"

#include "Game/Room.hpp"

namespace rtp::server
{

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    NetworkSyncSystem::NetworkSyncSystem(ServerNetwork &network,
                                         ecs::Registry &registry)
        : _network(network)
        , _registry(registry) {};

    void NetworkSyncSystem::update(float dt)
    {
        (void)dt;
    };

    void NetworkSyncSystem::bindSessionToEntity(uint32_t sessionId,
                                                ecs::Entity entity)
    {
        _sessionToEntity[sessionId] = entity;
    }

    void NetworkSyncSystem::unbindSession(uint32_t sessionId)
    {
        _sessionToEntity.erase(sessionId);
    }

    void NetworkSyncSystem::handleInput(uint32_t sessionId,
                                        const net::Packet &packet)
    {
        net::InputPayload payload;
        net::Packet tempPacket = packet;
        tempPacket >> payload;
        log::info("InputTick: session={} mask={}", sessionId,
                  (int)payload.inputMask);
        if (_sessionToEntity.find(sessionId) == _sessionToEntity.end()) {
            return;
        }

        ecs::Entity entity = _sessionToEntity[sessionId];

        auto inputsRes =
            _registry.get<ecs::components::server::InputComponent>();
        if (inputsRes) {
            auto &inputs = inputsRes->get();
            if (inputs.has(entity)) {
                inputs[entity].lastMask = inputs[entity].mask;
                inputs[entity].mask = payload.inputMask;
                return;
            }
        }

        ecs::components::server::InputComponent inputData{};
        inputData.mask = payload.inputMask;
        _registry.add<ecs::components::server::InputComponent>(entity,
                                                               inputData);
    }

    void NetworkSyncSystem::handleDisconnect(uint32_t sessionId)
    {
        if (_sessionToEntity.find(sessionId) != _sessionToEntity.end()) {
            ecs::Entity entity = _sessionToEntity[sessionId];
            _sessionToEntity.erase(sessionId);
            log::info("Destroyed entity {} for disconnected session {}",
                      entity.index(), sessionId);
        }
    }

    uint32_t
        NetworkSyncSystem::handlePlayerConnection(uint32_t sessionId,
                                                  const net::Packet &packet)
    {
        net::ConnectPayload payload;
        net::Packet tempPacket = packet;
        tempPacket >> payload;
        log::info("Player new connection [not logged]: session={}",
                  payload.sessionId);
        return {sessionId};
    }

    void NetworkSyncSystem::sendPacketToEntity(uint32_t entityId,
                                               const net::Packet &packet,
                                               net::NetworkMode mode)
    {
        for (const auto &[sessionId, entity] : _sessionToEntity) {
            if (entity.index() == entityId) {
                log::info("sendPacketToEntity: sending packet to session {} "
                          "for entity {}",
                          sessionId, entityId);
                _network.sendPacket(sessionId, packet, mode);
                return;
            }
        }
        log::warning("sendPacketToEntity: no session bound to entity {}",
                     entityId);
    }

    void NetworkSyncSystem::sendPacketToSession(uint32_t sessionId,
                                                const net::Packet &packet,
                                                net::NetworkMode mode)
    {
        _network.sendPacket(sessionId, packet, mode);
    }

    void NetworkSyncSystem::sendPacketToSessions(
        const std::vector<uint32_t> &sessions, const net::Packet &packet,
        net::NetworkMode mode)
    {
        for (uint32_t sessionId : sessions) {
            _network.sendPacket(sessionId, packet, mode);
        }
    }
}
