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

namespace rtp::server {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    LevelSystem::LevelSystem(rtp::ecs::Registry& registry,
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
            rtp::log::warning("No level path registered for level {}", levelId);
            return std::nullopt;
        }

        std::string error;
        auto level = loadLevelFromFile(it->second, error);
        if (!level) {
            rtp::log::error("Failed to load level {}: {}", levelId, error);
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
        rtp::log::info("Loaded level {} for room {}", levelId, roomId);
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
                    rtp::ecs::components::Transform,
                    rtp::ecs::components::EntityType,
                    rtp::ecs::components::RoomId
                >();
                for (auto &&[tf, type, room] : view) {
                    if (room.id != roomId || type.type != rtp::net::EntityType::Player) {
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
                rtp::Vec2f startPos = spawn.startPosition;
                const int patternIndex = static_cast<int>(active.nextSpawn % 5);
                const float xOffsets[5] = {0.0f, 100.0f, -40.0f, 80.0f, -80.0f};
                startPos.x += xOffsets[patternIndex];
                const float frontX = getFrontPlayerX();
                const float minAhead = 200.0f;
                if (startPos.x < frontX + minAhead) {
                    startPos.x = frontX + minAhead;
                }
                auto entity = _entitySystem.createEnemyEntity(
                    roomId, startPos, spawn.pattern,
                    spawn.speed, spawn.amplitude, spawn.frequency, spawn.type);
                spawnEntityForRoom(roomId, entity);
                active.nextSpawn++;
            }

            while (active.nextPowerup < active.data.powerups.size() &&
                active.data.powerups[active.nextPowerup].atTime <= active.elapsed) {
                const auto& powerup = active.data.powerups[active.nextPowerup];
                auto entity = _entitySystem.createPowerupEntity(
                    roomId, powerup.position, powerup.type, powerup.value, powerup.duration);
                _registry.add<rtp::ecs::components::Velocity>(
                    entity, rtp::ecs::components::Velocity{ {-scrollSpeed, 0.0f}, 0.0f }
                );
                spawnEntityForRoom(roomId, entity);
                active.nextPowerup++;
            }

            while (active.nextObstacle < active.data.obstacles.size() &&
                active.data.obstacles[active.nextObstacle].atTime <= active.elapsed) {
                const auto& obstacle = active.data.obstacles[active.nextObstacle];
                auto entity = _entitySystem.createObstacleEntity(
                    roomId, obstacle.position, obstacle.size, obstacle.health, obstacle.type);
                _registry.add<rtp::ecs::components::Velocity>(
                    entity, rtp::ecs::components::Velocity{ {-scrollSpeed, 0.0f}, 0.0f }
                );
                spawnEntityForRoom(roomId, entity);
                active.nextObstacle++;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    void LevelSystem::spawnEntityForRoom(uint32_t roomId, const rtp::ecs::Entity& entity)
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

        auto transformRes = _registry.get<rtp::ecs::components::Transform>();
        auto typeRes = _registry.get<rtp::ecs::components::EntityType>();
        auto netRes = _registry.get<rtp::ecs::components::NetworkId>();
        auto boxRes = _registry.get<rtp::ecs::components::BoundingBox>();
        if (!transformRes || !typeRes || !netRes) {
            rtp::log::error("Missing component array for level spawn");
            return;
        }

        auto &transforms = transformRes->get();
        auto &types = typeRes->get();
        auto &nets = netRes->get();
        auto *boxes = boxRes ? &boxRes->get() : nullptr;
        if (!transforms.has(entity) || !types.has(entity) || !nets.has(entity)) {
            rtp::log::error("Level spawned entity {} missing Transform/EntityType/NetworkId", entity.index());
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
        _networkSync.sendPacketToSessions(sessions, packet, rtp::net::NetworkMode::TCP);
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
