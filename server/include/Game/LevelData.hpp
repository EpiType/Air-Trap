/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** LevelData
*/

#pragma once

#include <optional>
#include <string>
#include <vector>

#include "RType/Math/Vec2.hpp"
#include "RType/Network/Packet.hpp"
#include "RType/ECS/Components/MouvementPattern.hpp"
#include "RType/ECS/Components/Powerup.hpp"

namespace rtp::server {

struct SpawnEvent {
    float atTime{0.0f};
    net::EntityType type{net::EntityType::Enemy1}; // Default remains Enemy1, Enemy4 is available
    Vec2f startPosition{0.0f, 0.0f};
    ecs::components::Patterns pattern{ecs::components::Patterns::StraightLine};
    float speed{120.0f};
    float amplitude{40.0f};
    float frequency{2.0f};
};

struct PowerupEvent {
    float atTime{0.0f};
    ecs::components::PowerupType type{ecs::components::PowerupType::Heal};
    Vec2f position{0.0f, 0.0f};
    float value{0.0f};
    float duration{0.0f};
};

struct ObstacleEvent {
    float atTime{0.0f};
    Vec2f position{0.0f, 0.0f};
    Vec2f size{32.0f, 32.0f};
    int health{50};
    net::EntityType type{net::EntityType::Obstacle};
};


struct BossPhase {
    float duration{0.0f};
    int spawnCount{0};
    float spawnInterval{0.0f};
    std::string enemyType;
    float spawnAreaX{1100.0f};
    float spawnAreaYMin{60.0f};
    float spawnAreaYMax{660.0f};
};

struct Boss3Data {
    std::vector<BossPhase> phases;
};

struct LevelData {
    uint32_t id{0};
    std::string name;
    Vec2f playerStart{100.0f, 100.0f};
    float widthPixels{0.0f};
    std::string tilesetPath;
    std::vector<SpawnEvent> spawns;
    std::vector<PowerupEvent> powerups;
    std::vector<ObstacleEvent> obstacles;
    // Optional boss3_invincible phase data
    std::optional<Boss3Data> boss3Data;
};

std::optional<LevelData> loadLevelFromFile(const std::string& path, std::string& error);

}  // namespace rtp::server
