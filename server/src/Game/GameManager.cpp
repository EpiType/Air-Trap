/**
 * File   : INetwork.hpp
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
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    GameManager::GameManager(ServerNetwork &networkManager)
        : _networkManager(networkManager)
    {        
        _registry.registerComponent<rtp::ecs::components::Transform>();
        _registry.registerComponent<rtp::ecs::components::Velocity>();
        _registry.registerComponent<rtp::ecs::components::NetworkId>();
        _registry.registerComponent<rtp::ecs::components::server::InputComponent>();

        _serverNetworkSystem = std::make_unique<ServerNetworkSystem>(_networkManager, _registry);
        _movementSystem = std::make_unique<MovementSystem>(_registry);
        
        auto lobby = std::make_shared<Room>(_nextRoomId++, "Global Lobby", 4);
        _rooms[lobby->getId()] = lobby;
        log::info("Default Lobby created with ID {}", lobby->getId());
    }

    GameManager::~GameManager()
    {
        log::info("GameManager destroyed");
    }

    void GameManager::gameLoop(void)
    {
        const float dt = 1.0f / 60.0f;

        while (true) {
            auto start = std::chrono::high_resolution_clock::now();

            processNetworkEvents();
                        
            _serverTick++;
            _movementSystem->update(dt);
            _serverNetworkSystem->broadcastWorldState(_serverTick);
            
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    void startGame(Room &room)
    {
        log::info("Starting game in Room ID {}", room.getId());
        /* Future feature */
    }

    //////////////////////////////////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////////////////////////////////

    void GameManager::processNetworkEvents(void)
    {
        while (auto optionalEvent = _networkManager.pollEvent()) {
            auto& event = optionalEvent.value();
            switch (event.packet.header.opCode) {
                case OpCode::None:
                    break;
                case OpCode::Hello:
                    handlePlayerConnect(event.sessionId);
                    break;
                case OpCode::Disconnect:
                    handlePlayerDisconnect(event.sessionId);
                    break;
                case OpCode::InputTick:
                    _serverNetworkSystem->handleInput(event.sessionId, event.packet);
                    break;
                default:
                    handlePacket(event.sessionId, event.packet);
                    break;
            }
        }
    }

    void GameManager::handlePlayerConnect(uint32_t sessionId)
    {
        log::info("Player with Session ID {} connected", sessionId);
        auto player = std::make_shared<Player>(sessionId, "Player_" + std::to_string(sessionId));
        
        {
            std::lock_guard lock(_mutex);
            _players[sessionId] = player;
        }

        tryJoinLobby(player);
    }

    void GameManager::handlePlayerDisconnect(uint32_t sessionId)
    {
        log::info("Player with Session ID {} disconnected", sessionId);
        std::lock_guard lock(_mutex);

        _players.erase(sessionId);

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
    }

    void GameManager::handlePacket(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        log::debug("Received OpCode {} from Session {}", (int)packet.header.opCode, sessionId);
    }

    void GameManager::spawnPlayerEntity(PlayerPtr player)
    {
        auto entityRes = _registry.spawnEntity();
        if (!entityRes) {
            log::error("Failed to spawn entity for player {}", player->getId());
            return;
        }
        log::info("Spawning entity for player {}", player->getId());
        auto entity = entityRes.value();

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
        
        player->setEntityId((uint32_t)entity);
        _serverNetworkSystem->bindSessionToEntity(player->getId(), (uint32_t)entity);

        log::info("Spawned Entity {} for Player {}", (uint32_t)entity, player->getId());
    }

    void GameManager::tryJoinLobby(PlayerPtr player, uint32_t roomId)
    {
        std::lock_guard lock(_mutex);

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
            
            std::this_thread::yield();

            sendLobbyUpdate(*targetRoom);

            spawnPlayerEntity(player);

            uint32_t entityId = player->getEntityId();
            _serverNetworkSystem->broadcastWorldState(_serverTick);

        } else {
             log::error("Failed to join Player {} to any room. Lobby is full.", player->getId());
        }
    }

    void GameManager::sendLobbyUpdate(const Room &room)
    {
        rtp::net::Packet updatePacket(rtp::net::OpCode::RoomUpdate);
        for (const auto& player : room.getPlayers()) {
            _networkManager.sendPacket(player->getId(), updatePacket, rtp::net::NetworkMode::TCP);
        }
    }
}