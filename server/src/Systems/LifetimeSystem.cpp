/**
 * File   : LifetimeSystem.cpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 18/01/2026
 */

#include "Systems/LifetimeSystem.hpp"
#include "RType/Logger.hpp"
#include "RType/Network/Packet.hpp"

#include <vector>

namespace rtp::server
{
    LifetimeSystem::LifetimeSystem(ecs::Registry& registry,
                                   NetworkSyncSystem& networkSync,
                                   RoomSystem& roomSystem)
        : _registry(registry)
        , _networkSync(networkSync)
        , _roomSystem(roomSystem)
    {
    }

    void LifetimeSystem::update(float dt)
    {
        using namespace rtp;
        auto lifetimesRes = _registry.get<ecs::components::Lifetime>();
        auto networkIdsRes = _registry.get<ecs::components::NetworkId>();
        auto roomIdsRes = _registry.get<ecs::components::RoomId>();

        if (!lifetimesRes || !networkIdsRes || !roomIdsRes) {
            return;
        }

        auto& lifetimes = lifetimesRes->get();
        auto& networkIds = networkIdsRes->get();
        auto& roomIds = roomIdsRes->get();

        std::vector<std::pair<ecs::Entity, uint32_t>> toDestroy;

        for (auto entity : lifetimes.entities()) {
            if (!lifetimes.has(entity)) {
                continue;
            }

            auto& lifetime = lifetimes[entity];
            lifetime.elapsed += dt;

            if (lifetime.elapsed >= lifetime.duration) {
                if (networkIds.has(entity) && roomIds.has(entity)) {
                    toDestroy.emplace_back(entity, roomIds[entity].id);
                }
            }
        }

        for (const auto& [entity, roomId] : toDestroy) {
            auto room = _roomSystem.getRoom(roomId);
            if (!room || room->getState() != Room::State::InGame) {
                continue;
            }

            const auto players = room->getPlayers();
            std::vector<uint32_t> sessions;
            sessions.reserve(players.size());
            for (const auto& player : players) {
                sessions.push_back(player->getId());
            }

            const uint32_t netId = networkIds[entity].id;
            net::Packet packet(net::OpCode::EntityDeath);
            net::EntityDeathPayload payload = { netId };
            packet << payload;
            _networkSync.sendPacketToSessions(sessions, packet, net::NetworkMode::TCP);

            _registry.kill(entity);
        }
    }
}
