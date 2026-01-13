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
    rtp::net::EntityType type{rtp::net::EntityType::Scout};
    rtp::Vec2f startPosition{0.0f, 0.0f};
    rtp::ecs::components::Patterns pattern{rtp::ecs::components::Patterns::StraightLine};
    float speed{120.0f};
    float amplitude{40.0f};
    float frequency{2.0f};
};

struct PowerupEvent {
    float atTime{0.0f};
    rtp::ecs::components::PowerupType type{rtp::ecs::components::PowerupType::Heal};
    rtp::Vec2f position{0.0f, 0.0f};
    float value{0.0f};
    float duration{0.0f};
};

struct ObstacleEvent {
    float atTime{0.0f};
    rtp::Vec2f position{0.0f, 0.0f};
    rtp::Vec2f size{32.0f, 32.0f};
    int health{50};
    rtp::net::EntityType type{rtp::net::EntityType::Obstacle};
};

struct LevelData {
    uint32_t id{0};
    std::string name;
    rtp::Vec2f playerStart{100.0f, 100.0f};
    float widthPixels{0.0f};
    std::string tilesetPath;
    std::vector<SpawnEvent> spawns;
    std::vector<PowerupEvent> powerups;
    std::vector<ObstacleEvent> obstacles;
};

std::optional<LevelData> loadLevelFromFile(const std::string& path, std::string& error);

}  // namespace rtp::server
