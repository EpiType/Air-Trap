/**
 * File   : MovementSystem.hpp
 * License: MIT
 * Author : Elias Josué
 * HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   :
 * 11/12/2025
 */

#include "Game/Room.hpp"
#include "Systems/NetworkSyncSystem.hpp"

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
                                                uint32_t entityId)
    {
        _sessionToEntity[sessionId] = entityId;
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

        uint32_t entityIndex = _sessionToEntity[sessionId];
        ecs::Entity entity(entityIndex, 0);

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
            uint32_t entityIndex = _sessionToEntity[sessionId];
            ecs::Entity entity(entityIndex, 0);
            _sessionToEntity.erase(sessionId);
            log::info("Destroyed entity {} for disconnected session {}",
                      entityIndex, sessionId);
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

}
