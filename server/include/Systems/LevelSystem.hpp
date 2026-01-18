/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** LevelSystem
*/

#pragma once

#include <string>
#include <unordered_map>
#include <optional>

#include "Game/LevelData.hpp"
#include "Systems/EntitySystem.hpp"
#include "Systems/RoomSystem.hpp"
#include "Systems/NetworkSyncSystem.hpp"

namespace rtp::server {

class LevelSystem {
    public:
        /**
         * @brief Constructor for LevelSystem
         * @param registry Reference to the ECS registry
         * @param entitySystem Reference to the EntitySystem
         * @param roomSystem Reference to the RoomSystem
         * @param networkSync Reference to the NetworkSyncSystem
         */
        LevelSystem(ecs::Registry& registry,
                    EntitySystem& entitySystem,
                    RoomSystem& roomSystem,
                    NetworkSyncSystem& networkSync);

        /**
         * @brief Register a level file path with a level ID
         * @param levelId ID of the level
         * @param path File path to the level data
         */
        void registerLevelPath(uint32_t levelId, const std::string& path);

        /**
         * @brief Start a level for a specific room
         * @param roomId ID of the room
         * @param levelId ID of the level to start
         */
        void startLevelForRoom(uint32_t roomId, uint32_t levelId);

        /**
         * @brief Stop the level for a specific room
         * @param roomId ID of the room
         */
        void stopLevelForRoom(uint32_t roomId);

        /**
         * @brief Update system logic for one frame
         * @param dt Time elapsed since last update in seconds
         */
        void update(float dt);

        /**
         * @brief Get the level data for a specific room
         * @param roomId ID of the room
         * @return Pointer to the LevelData if the level is active, nullptr otherwise
         */
        const LevelData* getLevelData(uint32_t roomId) const;

    private:
        /** @struct ActiveLevel
         *  @brief Struct to hold active level data for a room
         */
        struct ActiveLevel {
            LevelData data;
            float elapsed{0.0f};
            size_t nextSpawn{0};
            size_t nextPowerup{0};
            size_t nextObstacle{0};
            bool bossHasSpawnedInWorld{false};
            // Boss3Invincible logic
            bool boss3Active{false};
            float boss3Timer{0.0f};
            float boss3PhaseTimer{0.0f};
            int boss3PhaseIndex{0};
            float boss3NextEnemySpawn{0.0f};
            int boss3EnemiesSpawnedThisPhase{0};
        };

        /**
         * @brief Spawn an entity for a specific room
         * @param roomId ID of the room
         * @param entity Entity to spawn
         */
        void spawnEntityForRoom(uint32_t roomId, const ecs::Entity& entity);

        /**
         * @brief Load level data from a file
         * @param levelId ID of the level to load
         * @return Optional LevelData if loading was successful, std::nullopt otherwise
         */
        std::optional<LevelData> loadLevel(uint32_t levelId) const;
    
    private:
        ecs::Registry& _registry;      /**< Reference to the entity registry */
        EntitySystem& _entitySystem;        /**< Reference to the EntitySystem */
        RoomSystem& _roomSystem;            /**< Reference to the RoomSystem */
        NetworkSyncSystem& _networkSync;    /**< Reference to the NetworkSyncSystem */
        std::unordered_map<uint32_t,
            ActiveLevel> _activeLevels;     /**< Active levels mapped by room ID */
        std::unordered_map<uint32_t,
            std::string> _levelPaths;       /**< Level file paths mapped by level ID */
};

}  // namespace rtp::server
