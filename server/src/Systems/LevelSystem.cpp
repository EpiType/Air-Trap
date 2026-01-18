/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** LevelSystem
*/

#include "Systems/LevelSystem.hpp"
#include "RType/Logger.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/EntityType.hpp"
#include "RType/ECS/Components/RoomId.hpp"
#include "RType/ECS/Components/Velocity.hpp"

#include <cmath>

namespace rtp::server {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    LevelSystem::LevelSystem(ecs::Registry& registry,
                            EntitySystem& entitySystem,
                            RoomSystem& roomSystem,
                            NetworkSyncSystem& networkSync)
        : _registry(registry)
        , _entitySystem(entitySystem)
        , _roomSystem(roomSystem)
        , _networkSync(networkSync)
    {
    }

    void LevelSystem::registerLevelPath(uint32_t levelId, const std::string& path)
    {
        _levelPaths[levelId] = path;
    }

    std::optional<LevelData> LevelSystem::loadLevel(uint32_t levelId) const
    {
        auto it = _levelPaths.find(levelId);
        if (it == _levelPaths.end()) {
            log::warning("No level path registered for level {}", levelId);
            return std::nullopt;
        }

        std::string error;
        auto level = loadLevelFromFile(it->second, error);
        if (!level) {
            log::error("Failed to load level {}: {}", levelId, error);
            return std::nullopt;
        }

        return level;
    }

    void LevelSystem::startLevelForRoom(uint32_t roomId, uint32_t levelId)
    {
        auto level = loadLevel(levelId);
        if (!level) {
            return;
        }

        ActiveLevel active{};
        active.data = std::move(level.value());
        _activeLevels[roomId] = std::move(active);
        log::info("Loaded level {} for room {}", levelId, roomId);
    }

    void LevelSystem::stopLevelForRoom(uint32_t roomId)
    {
        _activeLevels.erase(roomId);
    }

    void LevelSystem::update(float dt)
    {
        constexpr float scrollSpeed = 120.0f;

        for (auto& [roomId, active] : _activeLevels) {
            auto room = _roomSystem.getRoom(roomId);
            if (!room || room->getState() != Room::State::InGame) {
                continue;
            }

            active.elapsed += dt;

            auto getFrontPlayerX = [&]() -> float {
                float maxX = 0.0f;
                auto view = _registry.zipView<
                    ecs::components::Transform,
                    ecs::components::EntityType,
                    ecs::components::RoomId
                >();
                for (auto &&[tf, type, room] : view) {
                    if (room.id != roomId || type.type != net::EntityType::Player) {
                        continue;
                    }
                    if (tf.position.x > maxX) {
                        maxX = tf.position.x;
                    }
                }
                return maxX;
            };

            while (active.nextSpawn < active.data.spawns.size() &&
                active.data.spawns[active.nextSpawn].atTime <= active.elapsed) {
                const auto& spawn = active.data.spawns[active.nextSpawn];
                Vec2f startPos = spawn.startPosition;
                const int patternIndex = static_cast<int>(active.nextSpawn % 5);
                const float xOffsets[5] = {0.0f, 100.0f, -40.0f, 80.0f, -80.0f};
                startPos.x += xOffsets[patternIndex];
                const float frontX = getFrontPlayerX();
                const bool isBoss = (spawn.type == net::EntityType::Boss ||
                                     spawn.type == net::EntityType::Boss2);
                const float minAhead = isBoss
                    ? 450.0f   // boss plus éloigné
                    : 200.0f;  // distance standard pour scouts/tanks/etc.
                if (startPos.x < frontX + minAhead) {
                    startPos.x = frontX + minAhead;
                }
                auto entity = _entitySystem.createEnemyEntity(
                    roomId, startPos, spawn.pattern,
                    spawn.speed, spawn.amplitude, spawn.frequency, spawn.type);
                spawnEntityForRoom(roomId, entity);
                
                // If spawning Boss1, also spawn 4 BossShields in a semi-circle in front of it
                if (spawn.type == net::EntityType::Boss) {
                    const float shieldRadius = 250.0f; // Distance from boss center (more separation)
                    // Semi-circle in front (left side, towards player): angles from 90° to 270°
                    const float angles[4] = {120.0f, 150.0f, 210.0f, 240.0f}; // 4 shields spread in front (left)
                    
                    for (int i = 0; i < 4; i++) {
                        float angleRad = angles[i] * 3.14159f / 180.0f;
                        Vec2f shieldPos = {
                            startPos.x + shieldRadius * std::cos(angleRad),
                            startPos.y + shieldRadius * std::sin(angleRad)
                        };
                        
                        auto shieldEntity = _entitySystem.createEnemyEntity(
                            roomId, shieldPos, spawn.pattern,
                            spawn.speed, spawn.amplitude, spawn.frequency, 
                            net::EntityType::BossShield);
                        spawnEntityForRoom(roomId, shieldEntity);
                    }
                }
                
                active.nextSpawn++;
            }

            while (active.nextPowerup < active.data.powerups.size() &&
                active.data.powerups[active.nextPowerup].atTime <= active.elapsed) {
                const auto& powerup = active.data.powerups[active.nextPowerup];
                auto entity = _entitySystem.createPowerupEntity(
                    roomId, powerup.position, powerup.type, powerup.value, powerup.duration);
                _registry.add<ecs::components::Velocity>(
                    entity, ecs::components::Velocity{ {-scrollSpeed, 0.0f}, 0.0f }
                );
                spawnEntityForRoom(roomId, entity);
                active.nextPowerup++;
            }

            while (active.nextObstacle < active.data.obstacles.size() &&
                active.data.obstacles[active.nextObstacle].atTime <= active.elapsed) {
                const auto& obstacle = active.data.obstacles[active.nextObstacle];
                auto entity = _entitySystem.createObstacleEntity(
                    roomId, obstacle.position, obstacle.size, obstacle.health, obstacle.type);
                _registry.add<ecs::components::Velocity>(
                    entity, ecs::components::Velocity{ {-scrollSpeed, 0.0f}, 0.0f }
                );
                spawnEntityForRoom(roomId, entity);
                active.nextObstacle++;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    void LevelSystem::spawnEntityForRoom(uint32_t roomId, const ecs::Entity& entity)
    {
        auto room = _roomSystem.getRoom(roomId);
        if (!room) {
            return;
        }

        const auto players = room->getPlayers();
        if (players.empty()) {
            return;
        }

        std::vector<uint32_t> sessions;
        sessions.reserve(players.size());
        for (const auto& player : players) {
            sessions.push_back(player->getId());
        }

        auto transformRes = _registry.get<ecs::components::Transform>();
        auto typeRes = _registry.get<ecs::components::EntityType>();
        auto netRes = _registry.get<ecs::components::NetworkId>();
        auto boxRes = _registry.get<ecs::components::BoundingBox>();
        if (!transformRes || !typeRes || !netRes) {
            log::error("Missing component array for level spawn");
            return;
        }

        auto &transforms = transformRes->get();
        auto &types = typeRes->get();
        auto &nets = netRes->get();
        auto *boxes = boxRes ? &boxRes->get() : nullptr;
        if (!transforms.has(entity) || !types.has(entity) || !nets.has(entity)) {
            log::error("Level spawned entity {} missing Transform/EntityType/NetworkId", entity.index());
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

        net::Packet packet(net::OpCode::EntitySpawn);
        net::EntitySpawnPayload payload = {
            net.id,
            static_cast<uint8_t>(type.type),
            transform.position.x,
            transform.position.y,
            sizeX,
            sizeY
        };
        packet << payload;
        _networkSync.sendPacketToSessions(sessions, packet, net::NetworkMode::TCP);
    }

    const LevelData* LevelSystem::getLevelData(uint32_t roomId) const
    {
        auto it = _activeLevels.find(roomId);
        if (it == _activeLevels.end()) {
            return nullptr;
        }
        return &it->second.data;
    }

}  // namespace rtp::server
