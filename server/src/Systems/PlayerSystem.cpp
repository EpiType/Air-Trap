/**
 * File   : PlayerSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/PlayerSystem.hpp"

namespace rtp::server {
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    PlayerSystem::PlayerSystem(ServerNetwork& network, rtp::ecs::Registry& registry)
        : _network(network), _registry(registry) {}

    void PlayerSystem::update(float dt) 
    {
        (void)dt;
    };

    PlayerPtr PlayerSystem::createPlayer(uint32_t sessionId, const std::string& username) 
    {
        auto it = _players.find(sessionId);
        if (it == _players.end()) {
            log::warning("Auth: sessionId {} not in _players yet, creating Player now", sessionId);
            _players[sessionId] = std::make_shared<Player>(sessionId);
            it = _players.find(sessionId);
        }
        it->second->setUsername(username);
        rtp::log::info("Player created for Session ID {}", sessionId);
        return it->second;
    }

    uint32_t PlayerSystem::removePlayer(uint32_t sessionId)
    {
        uint32_t entityId = 0;

        auto itP = _players.find(sessionId); 
        if (itP != _players.end()) {
            entityId = itP->second->getEntityId();
        }
        _players.erase(sessionId);
        rtp::log::info("Player removed for Session ID {}", sessionId);
        return entityId;
    }

    void PlayerSystem::updatePlayerUsername(uint32_t sessionId, const std::string& username) 
    {
        auto it = _players.find(sessionId);
        if (it != _players.end()) {
            it->second->setUsername(username);
            rtp::log::info("Updated username for Session ID {}: {}", sessionId, username);
        }
    }

    PlayerPtr PlayerSystem::getPlayer(uint32_t sessionId) const
    {
        auto it = _players.find(sessionId);
        if (it != _players.end()) {
            return it->second;
        }
        return nullptr;
    }

    PlayerPtr PlayerSystem::getPlayerByUsername(const std::string& username) const
    {
        for (const auto& [sid, player] : _players) {
            if (player && player->getUsername() == username) {
                return player;
            }
        }
        return nullptr;
    }
} // namespace rtp::server
