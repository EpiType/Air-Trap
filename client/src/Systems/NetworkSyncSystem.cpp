/**
 * File   : NetworkSyncSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "Systems/NetworkSyncSystem.hpp"
#include "RType/Logger.hpp"
#include "RType/ECS/Components/NetworkId.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Animation.hpp"
#include "RType/ECS/Components/EntityType.hpp"
#include "RType/Network/Packet.hpp"
#include "Game/EntityBuilder.hpp"

using namespace rtp::net;

namespace rtp::client {
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    NetworkSyncSystem::NetworkSyncSystem(ClientNetwork& network, rtp::ecs::Registry& registry, Client::Game::EntityBuilder builder)
        : _network(network), _registry(registry), _builder(builder) {}

    void NetworkSyncSystem::update(float dt)
    {
        (void)dt;
        while (auto eventOpt = _network.pollEvent()) {
            handleEvent(*eventOpt);
        }
    }

    void NetworkSyncSystem::tryLogin(const std::string& username, const std::string& password) const
    {
        rtp::net::LoginPayload payload;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);

        rtp::net::Packet packet(rtp::net::OpCode::LoginRequest);
        packet << payload;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryRegister(const std::string& username, const std::string& password) const
    {
        rtp::net::RegisterPayload payload;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        std::strncpy(payload.password, password.c_str(), sizeof(payload.password) - 1);

        rtp::net::Packet packet(rtp::net::OpCode::RegisterRequest);
        packet << payload;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::requestListRooms(void)
    {
        rtp::log::info("Requesting list of available rooms from server...");
        if (_availableRooms.size() > 0)
            _availableRooms.clear();
        rtp::net::Packet packet(rtp::net::OpCode::ListRooms);
        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryCreateRoom(const std::string& roomName, uint32_t maxPlayers, float difficulty, float speed, uint32_t duration, uint32_t seed, uint32_t levelId)
    {
        rtp::log::info("Attempting to create room '{}' on server...", roomName);
        _currentState = State::CreatingRoom;
        rtp::net::CreateRoomPayload payload;
        std::strncpy(payload.roomName, roomName.c_str(), sizeof(payload.roomName) - 1);
        payload.maxPlayers = maxPlayers;
        payload.difficulty = difficulty;
        payload.speed = speed;
        payload.duration = duration;
        payload.seed = seed;
        payload.levelId = levelId;

        rtp::net::Packet packet(rtp::net::OpCode::CreateRoom);
        packet << payload;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryJoinRoom(uint32_t roomId)
    {
        _currentState = State::JoiningRoom;
        rtp::net::Packet packet(rtp::net::OpCode::JoinRoom);
        packet << roomId;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    void NetworkSyncSystem::tryLeaveRoom(void)
    {
        _currentState = State::LeavingRoom;
        rtp::net::Packet packet(rtp::net::OpCode::LeaveRoom);
        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
        trySetReady(false);
    }

    void NetworkSyncSystem::trySetReady(bool isReady)
    {
        rtp::net::Packet packet(rtp::net::OpCode::SetReady);
        packet << static_cast<uint8_t>(isReady ? 1 : 0);

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }
    
    void NetworkSyncSystem::trySendMessage(const std::string& message) const
    {
        rtp::net::Packet packet(rtp::net::OpCode::RoomChatSended);
        packet << message;

        _network.sendPacket(packet, rtp::net::NetworkMode::TCP);
    }

    //////////////////////////////////////////////////////////////////////////
    // Getters
    //////////////////////////////////////////////////////////////////////////

    bool NetworkSyncSystem::isInRoom(void) const
    {
        return _currentState == State::InRoom || _currentState == State::InGame;
    }

    bool NetworkSyncSystem::isReady(void) const
    {
        return _isReady;
    }

    bool NetworkSyncSystem::isUdpReady(void) const
    {
        return _network.isUdpReady();
    }

    bool NetworkSyncSystem::isLoggedIn(void) const
    {
        return _isLoggedIn;
    }

    std::string NetworkSyncSystem::getUsername(void) const
    {
        return _username;
    }

    std::list<rtp::net::RoomInfo> NetworkSyncSystem::getAvailableRooms(void) const
    {
        return _availableRooms;
    }

    bool NetworkSyncSystem::isInGame(void) const
    {
        return _currentState == State::InGame;
    }

    //////////////////////////////////////////////////////////////////////////
    // Private Methods
    //////////////////////////////////////////////////////////////////////////

    void NetworkSyncSystem::handleEvent(rtp::net::NetworkEvent& event)
    {
        switch (event.packet.header.opCode)
        {
            // case OpCode::EntitySpawn: { /* EntitySpawn */
            //     rtp::net::EntitySpawnPayload payload;
            //     event.packet >> payload;
            //     spawnEntityFromServer(payload);
            //     break;
            // }
            case OpCode::LoginResponse: {
                onLoginResponse(event.packet);
                break;
            }
            case OpCode::RegisterResponse: {
                onRegisterResponse(event.packet);
                break;
            }
            case OpCode::RoomList: {
                onRoomResponse(event.packet);
                break;
            }
            case OpCode::JoinRoom: {
                onJoinRoomResponse(event.packet);
                break;
            }
            case OpCode::CreateRoom: {
                onCreateRoomResponse(event.packet);
                break;
            }
            case OpCode::LeaveRoom: {
                onLeaveRoomResponse(event.packet);
                break;
            }
            case OpCode::StartGame: {
                rtp::log::info("Received StartGame notification from server.");
                _currentState = State::InGame;
                break;
            }
            case OpCode::EntitySpawn: {
                onSpawnEntityFromServer(event.packet);
                break;
            }
            case OpCode::RoomUpdate:
                onRoomUpdate(event.packet);
                break;
            default:
                rtp::log::warning("Unhandled OpCode received: {}", static_cast<uint8_t>(event.packet.header.opCode));
                break;
            // case OpCode::EntityDeath: /* EntityDeath */
            //     /* code */
            //     break;
            // case OpCode::RoomUpdate: /* WorldUpdate */
            //     applyWorldUpdate(event.packet);
            //     break;
            // case OpCode::Disconnect: {/* Disconnect */
            //     uint32_t entityNetId = 0;
            //     event.packet >> entityNetId;
            //     disconnectPlayer(entityNetId);
            //     break;
            // }
        }
    }

    void NetworkSyncSystem::onLoginResponse(rtp::net::Packet& packet)
    {
        rtp::net::LoginResponsePayload payload;
        packet >> payload;

        if (payload.success) {
            rtp::log::info("Login successful for user '{}'", std::string(payload.username));
            _isLoggedIn = true;
            _username = std::string(payload.username);
            _currentState = State::InLobby;
        } else {
            rtp::log::warning("Login failed for user '{}'", std::string(payload.username));
            _isLoggedIn = false;
            _username = "";
            _currentState = State::NotLogged;
        }
    }

    void NetworkSyncSystem::onRegisterResponse(rtp::net::Packet& packet)
    {
        rtp::net::RegisterResponsePayload payload;
        packet >> payload;

        if (payload.success) {
            rtp::log::info("Registration successful for user '{}'", std::string(payload.username));
            _isLoggedIn = true;
            _username = std::string(payload.username);
            _currentState = State::InLobby;
        } else {
            rtp::log::warning("Registration failed for user '{}'", std::string(payload.username));
            _isLoggedIn = false;
            _username = "";
            _currentState = State::NotLogged;
        }
    }

    void NetworkSyncSystem::onRoomResponse(rtp::net::Packet& packet)
    {
        _availableRooms.clear();

        uint32_t roomCount = 0;
        packet >> roomCount;

        rtp::log::info("Received {} available rooms from server.", roomCount);

        if (roomCount <= 0) {
            rtp::log::info("No available rooms received from server.");
            return;
        }

        for (uint32_t i = 0; i < roomCount; ++i) {
            rtp::net::RoomInfo roomInfo{};
            packet >> roomInfo;
            _availableRooms.push_back(roomInfo);

            rtp::log::info(
                "Received Room: ID={} Name='{}' Players={}/{} InGame={} Difficulty={} Speed={} Duration={} Seed={} LevelID={}",
                roomInfo.roomId,
                std::string(roomInfo.roomName),
                roomInfo.currentPlayers,
                roomInfo.maxPlayers,
                (int)roomInfo.inGame,
                roomInfo.difficulty,
                roomInfo.speed,
                roomInfo.duration,
                roomInfo.seed,
                roomInfo.levelId
            );
        }
    }


    void NetworkSyncSystem::onJoinRoomResponse(rtp::net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;

        if (success) {
            rtp::log::info("Successfully joined the room.");
            _currentState = State::InRoom;
        } else {
            rtp::log::warning("Failed to join the room.");
        }
    }

    void NetworkSyncSystem::onCreateRoomResponse(rtp::net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;

        if (success) {
            rtp::log::info("Room created successfully.");
            _currentState = State::InRoom;
        } else {
            rtp::log::warning("Failed to create the room.");
        }
    }

    void NetworkSyncSystem::onLeaveRoomResponse(rtp::net::Packet& packet)
    {
        uint8_t success = 0;
        packet >> success;

        if (success) {
            rtp::log::info("Successfully left the room.");
            _currentState = State::InLobby;
        } else {
            rtp::log::warning("Failed to leave the room.");
        }
    }

    void NetworkSyncSystem::onSpawnEntityFromServer(rtp::net::Packet& packet)
    {
        rtp::net::EntitySpawnPayload payload;
        packet >> payload;

        rtp::log::info("Spawning entity from server: NetID={}, Type={}, Position=({}, {})",
            payload.netId, static_cast<int>(payload.type), payload.posX, payload.posY);

        const rtp::Vec2f pos{payload.posX, payload.posY};

        Client::Game::EntityTemplate t;
        switch (static_cast<rtp::net::EntityType>(payload.type)) {
            case rtp::net::EntityType::Scout:
                t = Client::Game::EntityTemplate::rt2_4(pos);
                break;

            case rtp::net::EntityType::Player:
                t = Client::Game::EntityTemplate::rt1_1(pos);
                break;

            default:
                rtp::log::warning("Unknown entity type {}, fallback template", (int)payload.type);
                t = Client::Game::EntityTemplate::rt2_2(pos);
                break;
        }

        auto res = _builder.spawn(t);
        if (!res) {
            rtp::log::error("Failed to spawn entity from template: {}", res.error().message());
            return;
        }

        auto e = res.value();

        _registry.addComponent<rtp::ecs::components::NetworkId>(
            e, rtp::ecs::components::NetworkId{payload.netId}
        );

        _netIdToEntity[payload.netId] = e;
    }

    void NetworkSyncSystem::onRoomUpdate(rtp::net::Packet& packet)
    {
        rtp::net::RoomSnapshotPayload header{};
        std::vector<rtp::net::EntitySnapshotPayload> snapshots;

        packet >> header >> snapshots;

        for (const auto& snap : snapshots) {
            auto it = _netIdToEntity.find(snap.netId);
            if (it == _netIdToEntity.end()) {
                continue;
            }

            const rtp::ecs::Entity e = it->second;

            auto transformsOpt = _registry.getComponents<rtp::ecs::components::Transform>();
            if (!transformsOpt)
                continue;

            auto &transforms = transformsOpt.value().get();
            if (!transforms.has(e))
                continue;

            // rtp::log::debug("Updating entity NetID={} Position=({}, {}) Rotation={}",
            //     snap.netId, snap.position.x, snap.position.y, snap.rotation);
            transforms[e].position.x = snap.position.x;
            transforms[e].position.y = snap.position.y;
            transforms[e].rotation   = snap.rotation;
        }
    }

    // void NetworkSyncSystem::spawnEntityFromServer(const rtp::net::EntitySpawnPayload& payload)
    // {
    //     auto res = _registry.spawnEntity();
    //     if (!res) return;

    //     auto e = res.value();

    //     _registry.addComponent<rtp::ecs::components::NetworkId>(
    //         e, 
    //         rtp::ecs::components::NetworkId{payload.netId}
    //     );
    //     _registry.addComponent<rtp::ecs::components::Transform>(
    //         e, 
    //         rtp::ecs::components::Transform{payload.position, 0.f, {1.f, 1.f}}
    //     );

    //     _registry.addComponent<rtp::ecs::components::EntityType>(
    //         e, 
    //         rtp::ecs::components::EntityType{static_cast<rtp::net::EntityType>(payload.type)}
    //     );

    //     switch (static_cast<rtp::net::EntityType>(payload.type)) {
    //     case rtp::net::EntityType::Scout:
    //         addScoutSprite(e);
    //         break;
    //     default: 
    //         break;
    //     }

    //     _netIdToEntity[payload.netId] = e;
    // }

    // void NetworkSyncSystem::addScoutSprite(rtp::ecs::Entity entity)
    // {
    //     rtp::ecs::components::Sprite spriteData;
    //     spriteData.texturePath = "assets/sprites/r-typesheet2.gif";
    //     spriteData.rectLeft = 300;
    //     spriteData.rectTop = 71;
    //     spriteData.rectWidth = 30;
    //     spriteData.rectHeight = 18;
    //     spriteData.zIndex = 5;
    //     spriteData.red = 255;

    //     _registry.addComponent<rtp::ecs::components::Sprite>(
    //         entity, 
    //         spriteData
    //     );

    //     rtp::ecs::components::Animation animData;
    //     animData.frameLeft = 300;
    //     animData.frameTop = 71;
    //     animData.frameWidth = 30;
    //     animData.frameHeight = 18;
    //     animData.totalFrames = 6;
    //     animData.frameDuration = 0.1f;
    //     animData.currentFrame = 0;
    //     animData.elapsedTime = 0.0f;

    //     _registry.addComponent<rtp::ecs::components::Animation>(entity, animData);
    // }

    // void NetworkSyncSystem::disconnectPlayer(uint32_t entityNetId)
    // {
    //     auto it = _netIdToEntity.find(entityNetId);
    //     if (it == _netIdToEntity.end()) {
    //         rtp::log::warning("Disconnect: unknown entityNetId {}", entityNetId);
    //         return;
    //     }

    //     rtp::ecs::Entity e = it->second;

    //     rtp::log::info("Removing entity netId={} entity={}", entityNetId, (uint32_t)e);

    //     _registry.killEntity(e);
    //     _netIdToEntity.erase(it);
    // }

    // void NetworkSyncSystem::applyWorldUpdate(rtp::net::Packet& packet)
    // {
    //     rtp::net::RoomSnapshotPayload header;
    //     std::vector<rtp::net::EntitySnapshotPayload> snapshots;

    //     packet >> header >> snapshots;

    //     auto view = _registry.zipView<
    //         rtp::ecs::components::NetworkId,
    //         rtp::ecs::components::Transform
    //     >();

    //     for (const auto& snap : snapshots) {
    //         bool found = false;
    //         for (auto&& [netId, tf] : view) {
    //             if (netId.id == snap.netId) {
    //                 tf.position.x = snap.position.x;
    //                 tf.position.y = snap.position.y;
    //                 found = true;
    //                 break;
    //             }
    //         }
    //         if (!found) {
    //             std::cout << "Entity with NetworkId " << snap.netId << " not found in registry.\n";
    //             spawnEntity(snap);
    //         }
    //     }
    // }

    // void NetworkSyncSystem::spawnEntity(const rtp::net::EntitySnapshotPayload& snap)
    // {
    //     auto entityRes = _registry.spawnEntity();
        
    //     if (!entityRes) return;
        
    //     auto entity = entityRes.value();

    //     rtp::log::info("Spawning Entity NetID: {} at ({}, {})", snap.netId, snap.position.x, snap.position.y);

    //     _registry.addComponent<rtp::ecs::components::NetworkId>(
    //         entity, 
    //         rtp::ecs::components::NetworkId{snap.netId}
    //     );
                
    //     _registry.addComponent<rtp::ecs::components::Transform>(
    //         entity, 
    //         rtp::ecs::components::Transform{ snap.position, snap.rotation, {1.0f, 1.0f} }
    //     );

    //     rtp::ecs::components::Sprite spriteData;
    //     spriteData.texturePath = "assets/sprites/r-typesheet42.gif";
    //     spriteData.rectLeft = 0;
    //     spriteData.rectTop = 0;
    //     spriteData.rectWidth = 33;
    //     spriteData.rectHeight = 17;
    //     spriteData.zIndex = 10;
    //     spriteData.red = 255;

    //     _registry.addComponent<rtp::ecs::components::Sprite>(
    //         entity, 
    //         spriteData
    //     );

    //     rtp::ecs::components::Animation animData;
    //     animData.frameLeft = 0;
    //     animData.frameTop = 0;
    //     animData.frameWidth = 33;
    //     animData.frameHeight = 17;
    //     animData.totalFrames = 5;
    //     animData.frameDuration = 0.1f;
    //     animData.currentFrame = 0;
    //     animData.elapsedTime = 0.0f;

    //     _registry.addComponent<rtp::ecs::components::Animation>(entity, animData);

    //     _netIdToEntity[snap.netId] = entity;
    // }
} // namespace rtp::client