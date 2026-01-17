/**
 * File   : PlayerSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/PlayerSystem.hpp"

#include "engine/core/Logger.hpp"

namespace rtp::server::systems
{

    /////////////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////////////

    PlayerSystem::PlayerPtr PlayerSystem::createPlayer(uint32_t sessionId,
                                                       const std::string &username)
    {
        auto it = _players.find(sessionId);
        if (it != _players.end()) {
            return it->second;
        }
        auto player = std::make_shared<Player>(sessionId);
        player->setUsername(username);
        _players.emplace(sessionId, player);
        return player;
    }

    PlayerSystem::PlayerPtr PlayerSystem::getPlayer(uint32_t sessionId) const
    {
        auto it = _players.find(sessionId);
        if (it == _players.end()) {
            return nullptr;
        }
        return it->second;
    }

    PlayerSystem::PlayerPtr PlayerSystem::getPlayerByUsername(const std::string &username) const
    {
        for (const auto &[id, player] : _players) {
            (void)id;
            if (player && player->username() == username) {
                return player;
            }
        }
        return nullptr;
    }

    uint32_t PlayerSystem::removePlayer(uint32_t sessionId)
    {
        auto it = _players.find(sessionId);
        if (it == _players.end()) {
            return 0;
        }
        const uint32_t entityId = it->second ? it->second->entityId() : 0;
        _players.erase(it);
        return entityId;
    }

    void PlayerSystem::updatePlayerUsername(uint32_t sessionId, const std::string &username)
    {
        auto it = _players.find(sessionId);
        if (it == _players.end()) {
            return;
        }
        it->second->setUsername(username);
    }
}
