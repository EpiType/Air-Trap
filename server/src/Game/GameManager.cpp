/**
 * File   : GameManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/GameManager.hpp"
#include "RType/Logger.hpp"
#include <thread>
#include <chrono>

using namespace rtp::net;

namespace rtp::server
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    GameManager::GameManager(INetwork &networkManager)
        : _networkManager(networkManager)
    {
        log::info("GameManager created");
        
        auto lobby = std::make_shared<Room>(_nextRoomId++, "Global Lobby", 4);
        _rooms[lobby->getId()] = lobby;
        log::info("Default Lobby created with ID {}", lobby->getId());
    };

    GameManager::~GameManager()
    {
        log::info("GameManager destroyed");
    };

    void GameManager::gameLoop(void)
    {
        while (true) {
            processNetworkEvents();
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    // Private Methods
    ///////////////////////////////////////////////////////////////////////////

    void GameManager::processNetworkEvents(void)
    {
        rtp::net::NetworkEvent event;

        while (auto optionalEvent = _networkManager.pollEvent()) {
            event = std::move(optionalEvent.value());
            switch (event.packet.header.opCode) {
                case OpCode::None:
                    break;
                case OpCode::Hello:
                    handlePlayerConnect(event.sessionId);
                    break;
                case OpCode::Disconnect:
                    handlePlayerDisconnect(event.sessionId);
                    break;
                default:
                    handlePacket(event.sessionId, event.packet);
                    break;
            }
        }
    };

    void GameManager::handlePlayerConnect(uint32_t sessionId)
    {
        log::info("Player with Session ID {} connected", sessionId);
        auto player = std::make_shared<Player>(sessionId, "Player_" + std::to_string(sessionId));
        tryJoinLobby(player);
    };

    void GameManager::handlePlayerDisconnect(uint32_t sessionId)
    {
        log::info("Player with Session ID {} disconnected", sessionId);
        std::lock_guard lock(_mutex);

        auto it = _playerRoomMap.find(sessionId);
        if (it != _playerRoomMap.end()) {
            uint32_t roomId = it->second;
            auto roomIt = _rooms.find(roomId);
            if (roomIt != _rooms.end()) {
                roomIt->second->removePlayer(sessionId);
                sendLobbyUpdate(*roomIt->second);
            }
            _playerRoomMap.erase(it);
        }
    };

    void GameManager::handlePacket(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        
        log::debug("Received OpCode {} from Session {}", (int)packet.header.opCode, sessionId);

        // nrml traitement des inputs ou autre actions dans le jeu
    };

    void GameManager::tryJoinLobby(PlayerPtr player, uint32_t roomId)
    {
        std::lock_guard lock(_mutex);

        /* For the V1 the player can only join the lobby 1 */
        std::shared_ptr<Room> targetRoom = nullptr;
        if (roomId == 0) {
            if (auto it = _rooms.find(1); it != _rooms.end() && it->second->canJoin()) {
                 targetRoom = it->second;
            }
        }

        if (targetRoom != nullptr) {
            targetRoom->addPlayer(player);
            _playerRoomMap[player->getId()] = targetRoom->getId();
            
            player->setRoomId(targetRoom->getId());
            player->setState(PlayerState::InLobby);

            log::info("Player {} (Session ID {}) joined Room ID {}", 
                      player->getUsername(), player->getId(), targetRoom->getId());
            
            rtp::net::Packet welcomePacket(rtp::net::OpCode::Welcome);
            welcomePacket << player->getId();
            _networkManager.sendPacket(player->getId(), welcomePacket, rtp::net::NetworkMode::TCP);
            
            sendLobbyUpdate(*targetRoom);

        } else {
             log::error("Failed to join Player {} to any room. Lobby is full.", player->getId());
        }
    };

    void GameManager::sendLobbyUpdate(const Room &room)
    {
        rtp::net::Packet updatePacket(rtp::net::OpCode::RoomUpdate);
        
        for (const auto& player : room.getPlayers()) {
            _networkManager.sendPacket(player->getId(), updatePacket, rtp::net::NetworkMode::TCP);
        }
        log::debug("Sent RoomUpdate for Room ID {}", room.getId());
    };
} // namespace rtp::server