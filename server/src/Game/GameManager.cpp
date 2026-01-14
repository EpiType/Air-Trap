/**
 * File   : GameManager.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Game/GameManager.hpp"
#include "RType/ECS/Components/RoomId.hpp"

#include <cctype>
#include <cstring>
#include <unordered_map>

using namespace rtp::net;

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    GameManager::GameManager(ServerNetwork &networkManager)
        : _networkManager(networkManager)
    {        
        _registry.subscribe<rtp::ecs::components::Transform>();
        _registry.subscribe<rtp::ecs::components::Velocity>();
        _registry.subscribe<rtp::ecs::components::NetworkId>();
        _registry.subscribe<rtp::ecs::components::server::InputComponent>();
        _registry.subscribe<rtp::ecs::components::EntityType>();
        _registry.subscribe<rtp::ecs::components::RoomId>();
        _registry.subscribe<rtp::ecs::components::SimpleWeapon>();
        _registry.subscribe<rtp::ecs::components::Ammo>();
        _registry.subscribe<rtp::ecs::components::MouvementPattern>();
        _registry.subscribe<rtp::ecs::components::Health>();
        _registry.subscribe<rtp::ecs::components::BoundingBox>();
        _registry.subscribe<rtp::ecs::components::Damage>();
        _registry.subscribe<rtp::ecs::components::Powerup>();
        _registry.subscribe<rtp::ecs::components::MovementSpeed>();

        _networkSyncSystem = std::make_unique<NetworkSyncSystem>(_networkManager, _registry);
        _movementSystem = std::make_unique<MovementSystem>(_registry);
        _authSystem = std::make_unique<AuthSystem>(_networkManager, _registry);
        _roomSystem =  std::make_unique<RoomSystem>(_networkManager, _registry, *_networkSyncSystem);
        _playerSystem = std::make_unique<PlayerSystem>(_networkManager, _registry);
        _entitySystem = std::make_unique<EntitySystem>(_registry, _networkManager, *_networkSyncSystem);
        _playerMouvementSystem = std::make_unique<PlayerMouvementSystem>(_registry);
        _playerShootSystem = std::make_unique<PlayerShootSystem>(_registry, *_roomSystem, *_networkSyncSystem);
        _enemyAISystem = std::make_unique<EnemyAISystem>(_registry);
        _levelSystem = std::make_unique<LevelSystem>(_registry, *_entitySystem, *_roomSystem, *_networkSyncSystem);
        _collisionSystem = std::make_unique<CollisionSystem>(_registry, *_roomSystem, *_networkSyncSystem);
        _enemyShootSystem = std::make_unique<EnemyShootSystem>(_registry, *_roomSystem, *_networkSyncSystem);
        _bulletCleanupSystem = std::make_unique<BulletCleanupSystem>(_registry, *_roomSystem, *_networkSyncSystem);

        _levelSystem->registerLevelPath(1, "common/assets/levels/level_01.json");

        _roomSystem->setOnRoomStarted(
            [this](uint32_t roomId) {
                auto room = _roomSystem->getRoom(roomId);
                if (!room) {
                    rtp::log::error("Room ID {} not found for start event", roomId);
                    return;
                }

                _levelSystem->startLevelForRoom(roomId, room->getLevelId());

                const auto players = room->getPlayers();
                std::vector<uint32_t> sessions;
                sessions.reserve(players.size());
                for (const auto& player : players) {
                    sessions.push_back(player->getId());
                }

                const LevelData* level = _levelSystem->getLevelData(roomId);
                const rtp::Vec2f spawnPos = level
                    ? level->playerStart
                    : rtp::Vec2f{100.f, 100.f};

                for (auto& player : players) {
                    auto entity = _entitySystem->createPlayerEntity(player, spawnPos);
                    uint32_t entityId = static_cast<uint32_t>(entity.index());
                    player->setEntityId(entityId);
                    rtp::log::info("Spawned Entity {} for Player {}", entity.index(), player->getId());
                    _networkSyncSystem->bindSessionToEntity(player->getId(), entityId);
                    sendEntitySpawnToSessions(entity, sessions);
                }
            }
        );


        log::info("GameManager initialized");
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
            if (!_gamePaused) {
                const float scaledDt = dt * _gameSpeed;
                _roomSystem->update(scaledDt);
                _levelSystem->update(scaledDt);
                _enemyAISystem->update(scaledDt);
                _playerMouvementSystem->update(scaledDt);
                _playerShootSystem->update(scaledDt);
                _enemyShootSystem->update(scaledDt);
                _movementSystem->update(scaledDt);
                _collisionSystem->update(scaledDt);
                _bulletCleanupSystem->update(scaledDt);
            }
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
                case OpCode::LoginRequest:
                    handlePlayerLoginAuth(event.sessionId, event.packet);
                    break;
                case OpCode::RegisterRequest:
                    handlePlayerRegisterAuth(event.sessionId, event.packet);
                    break;
                case OpCode::InputTick:
                    _networkSyncSystem->handleInput(event.sessionId, event.packet);
                    break;
                case OpCode::ListRooms:
                    handleListRooms(event.sessionId);
                    break;
                case OpCode::CreateRoom:
                    handleCreateRoom(event.sessionId, event.packet);
                    break;
                case OpCode::JoinRoom:
                    handleJoinRoom(event.sessionId, event.packet);
                    break;
                case OpCode::LeaveRoom:
                    handleLeaveRoom(event.sessionId, event.packet);
                    break;
                case OpCode::SetReady:
                    handleSetReady(event.sessionId, event.packet);
                    break;
                case OpCode::RoomChatSended:
                    handleRoomChatSended(event.sessionId, event.packet);
                    break;
                case OpCode::Ping:
                    handlePing(event.sessionId, event.packet);
                    break;
                default:
                    handlePacket(event.sessionId, event.packet);
                    break;
            }
        }
    }

    void GameManager::handlePlayerConnect(uint32_t sessionId)
    {
        log::info("Client with Session ID {} connected, not logged yet", sessionId);
    }

    void GameManager::handlePlayerDisconnect(uint32_t sessionId)
    {
        uint32_t entityId = 0;

        {
            std::lock_guard lock(_mutex);
            entityId = _playerSystem->removePlayer(sessionId);
            _roomSystem->disconnectPlayer(sessionId);
            _networkSyncSystem->handleDisconnect(sessionId);
        }

        if (entityId != 0) {
            rtp::ecs::Entity entity(entityId, 0);
            auto transformsRes = _registry.get<rtp::ecs::components::Transform>();
            auto typesRes = _registry.get<rtp::ecs::components::EntityType>();
            auto netsRes = _registry.get<rtp::ecs::components::NetworkId>();
            auto roomsRes = _registry.get<rtp::ecs::components::RoomId>();

            if (transformsRes && typesRes && netsRes && roomsRes) {
                auto &transforms = transformsRes->get();
                auto &types = typesRes->get();
                auto &nets = netsRes->get();
                auto &rooms = roomsRes->get();
                if (transforms.has(entity) && types.has(entity) && nets.has(entity) && rooms.has(entity)) {
                    auto room = _roomSystem->getRoom(rooms[entity].id);
                    if (room) {
                        const auto players = room->getPlayers();
                        if (!players.empty()) {
                            rtp::net::Packet packet(rtp::net::OpCode::EntityDeath);
                            rtp::net::EntityDeathPayload payload{};
                            payload.netId = nets[entity].id;
                            payload.type = static_cast<uint8_t>(types[entity].type);
                            payload.position = transforms[entity].position;
                            packet << payload;

                            for (const auto& player : players) {
                                _networkSyncSystem->sendPacketToSession(player->getId(), packet, rtp::net::NetworkMode::TCP);
                            }
                        }
                    }
                }
            }

            _registry.kill(entity);

            rtp::net::Packet disconnectPlayer(rtp::net::OpCode::Disconnect);
            disconnectPlayer << entityId;
            _networkManager.broadcastPacket(disconnectPlayer, rtp::net::NetworkMode::UDP);
        }

        log::info("Player with Session ID {} disconnected", sessionId);
    }

    void GameManager::handlePlayerLoginAuth(uint32_t sessionId, const Packet &packet)
    {
        rtp::log::info("Handling authentication for Session ID {}", sessionId);
        auto res = _authSystem->handleLoginRequest(sessionId, packet);
        if (!res.first) return;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->createPlayer(sessionId, res.second);
        }

        if (_roomSystem->joinLobby(player)) {
            player->setState(PlayerState::InLobby);
        } else {
            rtp::log::error("Failed to join Lobby for Session ID {}", sessionId);
        }
    }

    void GameManager::handlePlayerRegisterAuth(uint32_t sessionId, const Packet &packet)
    {
        rtp::log::info("Handling registration for Session ID {}", sessionId);
        auto res = _authSystem->handleRegisterRequest(sessionId, packet);
        if (!res.first) return;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->createPlayer(sessionId, res.second);
        }

        if (_roomSystem->joinLobby(player)) {
            player->setState(PlayerState::InLobby);
        } else {
            rtp::log::error("Failed to join Lobby for Session ID {}", sessionId);
        }
    }

    void GameManager::handleListRooms(uint32_t sessionId)
    {
        _roomSystem->listAllRooms(sessionId);
    }

    void GameManager::handleCreateRoom(uint32_t sessionId, const Packet &packet)
    {
        CreateRoomPayload payload;
        Packet tmp = packet;
        tmp >> payload;

        PlayerPtr player;
        uint32_t newId = 0;
        uint8_t success = 0;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->getPlayer(sessionId);
            if (!player) {
                rtp::net::Packet response(rtp::net::OpCode::CreateRoom);
                response << static_cast<uint8_t>(0);
                _networkManager.sendPacket(sessionId, response, rtp::net::NetworkMode::TCP);
                return;
            }
            newId = _roomSystem->createRoom(
                sessionId,
                std::string(payload.roomName),
                static_cast<uint8_t>(payload.maxPlayers),
                payload.difficulty,
                payload.speed,
                Room::RoomType::Public,
                payload.levelId,
                payload.seed,
                payload.duration
            );
            success = (newId != 0) ? 1 : 0;
        }
        if (success) {
            success = _roomSystem->joinRoom(player, newId, false) ? 1 : 0;
        }
        rtp::net::Packet response(rtp::net::OpCode::CreateRoom);
        response << success;
        _networkManager.sendPacket(sessionId, response, rtp::net::NetworkMode::TCP);
        // entityId = _entitySystem->createPlayerEntity(player);
        // _serverNetworkSystem->bindSessionToEntity(player->getId(), entityId);
    }

    void GameManager::handleJoinRoom(uint32_t sessionId, const Packet &packet)
    {        
        rtp::net::JoinRoomPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->getPlayer(sessionId);
        }

        const bool success = _roomSystem->joinRoom(player, payload.roomId, payload.isSpectator != 0);
        if (!success)
            return;

        auto room = _roomSystem->getRoom(payload.roomId);
        if (!room)
            return;
        if (room->getState() != Room::State::InGame)
            return;

        rtp::net::Packet startPacket(rtp::net::OpCode::StartGame);
        _networkManager.sendPacket(sessionId, startPacket, rtp::net::NetworkMode::TCP);
        sendRoomEntitySpawnsToSession(payload.roomId, sessionId);
        // entityId = _entitySystem->createPlayerEntity(player);
        // _serverNetworkSystem->bindSessionToEntity(player->getId(), entityId);
    }

    void GameManager::handleLeaveRoom(uint32_t sessionId, const Packet &packet)
    {
        (void)packet;
        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->getPlayer(sessionId);
        }

        rtp::log::info("Handle Leave Room request from Session ID {}", sessionId);

        _roomSystem->leaveRoom(player);
    }

    void GameManager::handleSetReady(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        
        rtp::net::SetReadyPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;
        rtp::log::info("Handle Set Ready request from Session ID {}: isReady={}",
                  sessionId, payload.isReady);
        _playerSystem->getPlayer(sessionId)->setReady(payload.isReady != 0);
    }

    void GameManager::handleRoomChatSended(uint32_t sessionId, const Packet &packet)
    {
        rtp::net::RoomChatPayload payload;
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;

        PlayerPtr player;
        {
            std::lock_guard lock(_mutex);
            player = _playerSystem->getPlayer(sessionId);
        }
        if (!player)
            return;

        const uint32_t roomId = player->getRoomId();
        if (roomId == 0)
            return;

        auto room = _roomSystem->getRoom(roomId);
        if (!room)
            return;

        const std::string message(payload.message);
        if (handleChatCommand(player, message)) {
            return;
        }
        if (player->isMuted()) {
            return;
        }

        rtp::net::RoomChatReceivedPayload chatPayload{};
        chatPayload.sessionId = sessionId;
        const std::string username = player->getUsername();
        std::strncpy(chatPayload.username, username.c_str(), sizeof(chatPayload.username) - 1);
        chatPayload.username[sizeof(chatPayload.username) - 1] = '\0';
        std::strncpy(chatPayload.message, payload.message, sizeof(chatPayload.message) - 1);
        chatPayload.message[sizeof(chatPayload.message) - 1] = '\0';

        rtp::net::Packet chatPacket(rtp::net::OpCode::RoomChatReceived);
        chatPacket << chatPayload;

        const auto players = room->getPlayers();
        for (const auto& p : players) {
            _networkManager.sendPacket(p->getId(), chatPacket, rtp::net::NetworkMode::TCP);
        }
    }

    void GameManager::handlePacket(uint32_t sessionId, const Packet &packet)
    {
        std::lock_guard lock(_mutex);
        rtp::log::info("Received packet OpCode {} from Session {}", (int)rtp::net::OpCode::LoginRequest, sessionId);
        log::debug("Unknown packet received OpCode {} from Session {}", (int)packet.header.opCode, sessionId);
    }

    void GameManager::handlePing(uint32_t sessionId, const Packet &packet)
    {
        rtp::net::PingPayload payload{};
        rtp::net::Packet tempPacket = packet;
        tempPacket >> payload;

        rtp::net::Packet response(rtp::net::OpCode::Pong);
        response << payload;
        _networkManager.sendPacket(sessionId, response, rtp::net::NetworkMode::UDP);
    }

    bool GameManager::handleChatCommand(PlayerPtr player, const std::string& message)
    {
        if (message.empty() || message[0] != '/')
            return false;

        const uint32_t roomId = player->getRoomId();
        if (roomId == 0)
            return true;

        auto room = _roomSystem->getRoom(roomId);
        if (!room)
            return true;

        auto toLower = [](std::string s) {
            for (char &c : s) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
            return s;
        };

        const std::string msg = message;
        const std::string cmd = toLower(msg.substr(0, msg.find(' ')));
        const std::string arg = msg.find(' ') == std::string::npos
            ? ""
            : msg.substr(msg.find(' ') + 1);

        if (cmd == "/help") {
            sendChatToSession(player->getId(), "Commands: /help, /kick <name>, /ban <name>, /mute <name>,");
            sendChatToSession(player->getId(), "/stop, /run, /speed <float>, /debug <true|false>");
            sendChatToSession(player->getId(), "Examples: /speed 0.5 | /debug true | /kick player1");
            return true;
        }

        if (cmd == "/stop") {
            _gamePaused = true;
            sendSystemMessageToRoom(roomId, "Game paused");
            return true;
        }
        if (cmd == "/run") {
            _gamePaused = false;
            sendSystemMessageToRoom(roomId, "Game resumed");
            return true;
        }
        if (cmd == "/speed") {
            if (arg.empty()) {
                sendChatToSession(player->getId(), "Usage: /speed <float>");
                return true;
            }
            try {
                float v = std::stof(arg);
                if (v < 0.1f) v = 0.1f;
                if (v > 4.0f) v = 4.0f;
                _gameSpeed = v;
                sendSystemMessageToRoom(roomId, "Game speed set to " + std::to_string(v));
            } catch (...) {
                sendChatToSession(player->getId(), "Invalid speed value");
            }
            return true;
        }
        if (cmd == "/debug") {
            const std::string v = toLower(arg);
            if (v != "true" && v != "false") {
                sendChatToSession(player->getId(), "Usage: /debug <true|false>");
                return true;
            }
            const bool enabled = (v == "true");
            rtp::net::Packet packet(rtp::net::OpCode::DebugModeUpdate);
            rtp::net::DebugModePayload payload{ static_cast<uint8_t>(enabled ? 1 : 0) };
            packet << payload;
            const auto players = room->getPlayers();
            for (const auto& p : players) {
                _networkManager.sendPacket(p->getId(), packet, rtp::net::NetworkMode::TCP);
            }
            sendSystemMessageToRoom(roomId, std::string("Debug mode ") + (enabled ? "enabled" : "disabled"));
            return true;
        }

        if (cmd == "/kick" || cmd == "/mute" || cmd == "/ban") {
            if (arg.empty()) {
                sendChatToSession(player->getId(), std::string("Usage: ") + cmd + " <name>");
                return true;
            }
            PlayerPtr target = _playerSystem->getPlayerByUsername(arg);
            if (!target || target->getRoomId() != roomId) {
                sendChatToSession(player->getId(), "Player not found in room");
                return true;
            }
            if (cmd == "/kick") {
                _roomSystem->leaveRoom(target);
                rtp::net::Packet response(rtp::net::OpCode::LeaveRoom);
                response << static_cast<uint8_t>(1);
                _networkManager.sendPacket(target->getId(), response, rtp::net::NetworkMode::TCP);
                rtp::net::Packet kickedPacket(rtp::net::OpCode::Kicked);
                _networkManager.sendPacket(target->getId(), kickedPacket, rtp::net::NetworkMode::TCP);
                sendSystemMessageToRoom(roomId, target->getUsername() + " has been kicked");
                return true;
            }
            if (cmd == "/ban") {
                room->banUser(arg);
                _roomSystem->leaveRoom(target);
                rtp::net::Packet response(rtp::net::OpCode::LeaveRoom);
                response << static_cast<uint8_t>(1);
                _networkManager.sendPacket(target->getId(), response, rtp::net::NetworkMode::TCP);
                rtp::net::Packet kickedPacket(rtp::net::OpCode::Kicked);
                _networkManager.sendPacket(target->getId(), kickedPacket, rtp::net::NetworkMode::TCP);
                sendSystemMessageToRoom(roomId, target->getUsername() + " has been banned");
                return true;
            }
            if (cmd == "/mute") {
                const bool newMuted = !target->isMuted();
                target->setMuted(newMuted);
                sendSystemMessageToRoom(roomId,
                    target->getUsername() + (newMuted ? " has been muted" : " has been unmuted"));
                return true;
            }
        }

        sendChatToSession(player->getId(), "Unknown command. Use /help");
        return true;
    }

    void GameManager::sendChatToSession(uint32_t sessionId, const std::string& message)
    {
        rtp::net::RoomChatReceivedPayload payload{};
        payload.sessionId = 0;
        payload.username[0] = '\0';
        std::strncpy(payload.message, message.c_str(), sizeof(payload.message) - 1);
        payload.message[sizeof(payload.message) - 1] = '\0';

        rtp::net::Packet packet(rtp::net::OpCode::RoomChatReceived);
        packet << payload;
        _networkManager.sendPacket(sessionId, packet, rtp::net::NetworkMode::TCP);
    }

    void GameManager::sendSystemMessageToRoom(uint32_t roomId, const std::string& message)
    {
        auto room = _roomSystem->getRoom(roomId);
        if (!room)
            return;
        const auto players = room->getPlayers();

        rtp::net::RoomChatReceivedPayload payload{};
        payload.sessionId = 0;
        payload.username[0] = '\0';
        std::strncpy(payload.message, message.c_str(), sizeof(payload.message) - 1);
        payload.message[sizeof(payload.message) - 1] = '\0';

        rtp::net::Packet packet(rtp::net::OpCode::RoomChatReceived);
        packet << payload;

        for (const auto& p : players) {
            _networkManager.sendPacket(p->getId(), packet, rtp::net::NetworkMode::TCP);
        }
    }

    void GameManager::sendEntitySpawnToSessions(const rtp::ecs::Entity& entity,
                                                const std::vector<uint32_t>& sessions)
    {
        if (sessions.empty())
            return;

        auto transformRes = _registry.get<rtp::ecs::components::Transform>();
        auto typeRes = _registry.get<rtp::ecs::components::EntityType>();
        auto netRes = _registry.get<rtp::ecs::components::NetworkId>();
        auto boxRes = _registry.get<rtp::ecs::components::BoundingBox>();
        if (!transformRes || !typeRes || !netRes) {
            rtp::log::error("Missing component array for EntitySpawn");
            return;
        }

        auto &transforms = transformRes->get();
        auto &types = typeRes->get();
        auto &nets = netRes->get();
        auto *boxes = boxRes ? &boxRes->get() : nullptr;
        if (!transforms.has(entity) || !types.has(entity) || !nets.has(entity)) {
            rtp::log::error("Entity {} missing Transform/EntityType/NetworkId", entity.index());
            return;
        }

        const auto &transform = transforms[entity];
        const auto &type = types[entity];
        const auto &net = nets[entity];
        float sizeX = 0.0f;
        float sizeY = 0.0f;
        if (boxes && boxes->has(entity)) {
            const auto &box = (*boxes)[entity];
            sizeX = box.width;
            sizeY = box.height;
        }
        rtp::net::Packet packet(rtp::net::OpCode::EntitySpawn);
        rtp::net::EntitySpawnPayload payload = {
            net.id,
            static_cast<uint8_t>(type.type),
            transform.position.x,
            transform.position.y,
            sizeX,
            sizeY
        };
        packet << payload;
        _networkSyncSystem->sendPacketToSessions(sessions, packet, rtp::net::NetworkMode::TCP);
    }

    void GameManager::sendRoomEntitySpawnsToSession(uint32_t roomId, uint32_t sessionId)
    {
        auto view = _registry.zipView<
            rtp::ecs::components::Transform,
            rtp::ecs::components::NetworkId,
            rtp::ecs::components::EntityType,
            rtp::ecs::components::RoomId
        >();
        auto boxRes = _registry.get<rtp::ecs::components::BoundingBox>();
        auto *boxes = boxRes ? &boxRes->get() : nullptr;
        std::unordered_map<uint32_t, rtp::ecs::Entity> netToEntity;
        if (boxes) {
            auto netRes = _registry.get<rtp::ecs::components::NetworkId>();
            if (netRes) {
                auto &nets = netRes->get();
                for (auto e : nets.entities()) {
                    netToEntity[nets[e].id] = e;
                }
            }
        }

        for (auto&& [tf, net, type, room] : view) {
            if (room.id != roomId)
                continue;

            float sizeX = 0.0f;
            float sizeY = 0.0f;
            if (boxes) {
                auto it = netToEntity.find(net.id);
                if (it != netToEntity.end() && boxes->has(it->second)) {
                    const auto &box = (*boxes)[it->second];
                    sizeX = box.width;
                    sizeY = box.height;
                }
            }

            rtp::net::Packet packet(rtp::net::OpCode::EntitySpawn);
            rtp::net::EntitySpawnPayload payload = {
                net.id,
                static_cast<uint8_t>(type.type),
                tf.position.x,
                tf.position.y,
                sizeX,
                sizeY
            };
            packet << payload;
            _networkSyncSystem->sendPacketToSession(sessionId, packet, rtp::net::NetworkMode::TCP);
        }
    }
}
