/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** LevelData
*/

#include "Game/LevelData.hpp"
#include "RType/Logger.hpp"

#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <fstream>
#include <regex>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

namespace {
    std::string toLower(std::string value)
    {
        std::transform(value.begin(), value.end(), value.begin(),
            [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return value;
    }

    std::optional<std::string> readFile(const std::string& path)
    {
        std::ifstream in(path);
        if (!in) {
            return std::nullopt;
        }
        std::ostringstream ss;
        ss << in.rdbuf();
        return ss.str();
    }

    size_t findMatching(const std::string& text, size_t start, char open, char close)
    {
        bool inString = false;
        int depth = 1;

        for (size_t i = start + 1; i < text.size(); ++i) {
            const char c = text[i];
            if (c == '"' && (i == 0 || text[i - 1] != '\\')) {
                inString = !inString;
                continue;
            }
            if (inString) {
                continue;
            }
            if (c == open) {
                depth++;
            } else if (c == close) {
                depth--;
                if (depth == 0) {
                    return i;
                }
            }
        }
        return std::string::npos;
    }

    bool extractObject(const std::string& text, const std::string& key, std::string& out)
    {
        const std::string needle = "\"" + key + "\"";
        const size_t keyPos = text.find(needle);
        if (keyPos == std::string::npos) {
            return false;
        }
        const size_t bracePos = text.find('{', keyPos);
        if (bracePos == std::string::npos) {
            return false;
        }
        const size_t endPos = findMatching(text, bracePos, '{', '}');
        if (endPos == std::string::npos) {
            return false;
        }
        out = text.substr(bracePos, endPos - bracePos + 1);
        return true;
    }

    bool extractArray(const std::string& text, const std::string& key, std::string& out)
    {
        const std::string needle = "\"" + key + "\"";
        const size_t keyPos = text.find(needle);
        if (keyPos == std::string::npos) {
            return false;
        }
        const size_t startPos = text.find('[', keyPos);
        if (startPos == std::string::npos) {
            return false;
        }
        const size_t endPos = findMatching(text, startPos, '[', ']');
        if (endPos == std::string::npos) {
            return false;
        }
        out = text.substr(startPos, endPos - startPos + 1);
        return true;
    }

    std::vector<std::string> splitObjects(const std::string& arrayText)
    {
        std::vector<std::string> out;
        bool inString = false;
        int depth = 0;
        size_t start = std::string::npos;

        for (size_t i = 0; i < arrayText.size(); ++i) {
            const char c = arrayText[i];
            if (c == '"' && (i == 0 || arrayText[i - 1] != '\\')) {
                inString = !inString;
                continue;
            }
            if (inString) {
                continue;
            }
            if (c == '{') {
                if (depth == 0) {
                    start = i;
                }
                depth++;
            } else if (c == '}') {
                depth--;
                if (depth == 0 && start != std::string::npos) {
                    out.emplace_back(arrayText.substr(start, i - start + 1));
                    start = std::string::npos;
                }
            }
        }
        return out;
    }

    bool extractString(const std::string& text, const std::string& key, std::string& out)
    {
        const std::regex re("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
        std::smatch match;
        if (std::regex_search(text, match, re) && match.size() >= 2) {
            out = match[1].str();
            return true;
        }
        return false;
    }

    bool extractNumber(const std::string& text, const std::string& key, float& out)
    {
        const std::regex re("\"" + key + "\"\\s*:\\s*([-0-9.+eE]+)");
        std::smatch match;
        if (std::regex_search(text, match, re) && match.size() >= 2) {
            out = std::strtof(match[1].str().c_str(), nullptr);
            return true;
        }
        return false;
    }

    bool extractInt(const std::string& text, const std::string& key, int& out)
    {
        float value = 0.0f;
        if (!extractNumber(text, key, value)) {
            return false;
        }
        out = static_cast<int>(value);
        return true;
    }

    bool extractVec2(const std::string& text, const std::string& key, rtp::Vec2f& out)
    {
        std::string obj;
        if (!extractObject(text, key, obj)) {
            return false;
        }
        float x = 0.0f;
        float y = 0.0f;
        if (!extractNumber(obj, "x", x) || !extractNumber(obj, "y", y)) {
            return false;
        }
        out = {x, y};
        return true;
    }

    bool extractVec2WH(const std::string& text, const std::string& key, rtp::Vec2f& out)
    {
        std::string obj;
        if (!extractObject(text, key, obj)) {
            return false;
        }
        float w = 0.0f;
        float h = 0.0f;
        if (!extractNumber(obj, "w", w) || !extractNumber(obj, "h", h)) {
            return false;
        }
        out = {w, h};
        return true;
    }

    rtp::ecs::components::Patterns parsePattern(const std::string& value)
    {
        const std::string lower = toLower(value);
        if (lower == "sine" || lower == "sin") {
            return rtp::ecs::components::Patterns::SineWave;
        }
        if (lower == "static") {
            return rtp::ecs::components::Patterns::Static;
        }
        if (lower == "circular" || lower == "circle") {
            return rtp::ecs::components::Patterns::Circular;
        }
        if (lower == "zigzag" || lower == "zig-zag") {
            return rtp::ecs::components::Patterns::ZigZag;
        }
        return rtp::ecs::components::Patterns::StraightLine;
    }

    rtp::net::EntityType parseEntityType(const std::string& value, const std::string& templatePath)
    {
        const std::string lower = toLower(value);
        if (lower == "tank") {
            return rtp::net::EntityType::Tank;
        }
        if (lower == "boss") {
            return rtp::net::EntityType::Boss;
        }
        if (lower == "boss2") {
            return rtp::net::EntityType::Boss2;
        }
        if (lower == "boss_shield") {
            return rtp::net::EntityType::BossShield;
        }
        if (lower == "scout") {
            return rtp::net::EntityType::Scout;
        }

        const std::string templateLower = toLower(templatePath);
        if (templateLower.find("boss_02") != std::string::npos || templateLower.find("boss2") != std::string::npos) {
            return rtp::net::EntityType::Boss2;
        }
        if (templateLower.find("boss") != std::string::npos) {
            return rtp::net::EntityType::Boss;
        }
        if (templateLower.find("tank") != std::string::npos) {
            return rtp::net::EntityType::Tank;
        }
        return rtp::net::EntityType::Scout;
    }

    rtp::ecs::components::PowerupType parsePowerupType(const std::string& value)
    {
        const std::string lower = toLower(value);
        if (lower == "speed") {
            return rtp::ecs::components::PowerupType::Speed;
        }
        return rtp::ecs::components::PowerupType::Heal;
    }

    bool extractIntArray(const std::string& text, const std::string& key, std::vector<int>& out)
    {
        std::string arrayText;
        if (!extractArray(text, key, arrayText)) {
            return false;
        }

        const std::regex re("(-?\\d+)");
        auto begin = std::sregex_iterator(arrayText.begin(), arrayText.end(), re);
        auto end = std::sregex_iterator();
        for (auto it = begin; it != end; ++it) {
            out.push_back(std::stoi((*it)[1].str()));
        }
        return !out.empty();
    }

    enum class TileKind {
        None,
        Solid,
        Destructible
    };

    TileKind getTileKind(const std::string& tileObj)
    {
        const bool isSolid = tileObj.find("\"is_solid\"") != std::string::npos &&
            tileObj.find("true") != std::string::npos;
        const bool isDestructible = tileObj.find("\"is_destructible\"") != std::string::npos &&
            tileObj.find("true") != std::string::npos;

        if (isDestructible) {
            return TileKind::Destructible;
        }
        if (isSolid) {
            return TileKind::Solid;
        }
        return TileKind::None;
    }

    void appendTileObstacles(const std::string& tilesetPath,
                             std::vector<rtp::server::ObstacleEvent>& obstacles)
    {
        auto data = readFile(tilesetPath);
        if (!data) {
            rtp::log::warning("Failed to read tileset file: {}", tilesetPath);
            return;
        }

        const std::string& json = data.value();

        int height = 0;
        int tileWidth = 32;
        int tileHeight = 32;
        extractInt(json, "height", height);
        extractInt(json, "tilewidth", tileWidth);
        extractInt(json, "tileheight", tileHeight);

        std::unordered_map<int, TileKind> gidKinds;
        std::string tilesetsArray;
        if (extractArray(json, "tilesets", tilesetsArray)) {
            const auto tilesets = splitObjects(tilesetsArray);
            for (const auto& tilesetObj : tilesets) {
                int firstGid = 1;
                extractInt(tilesetObj, "firstgid", firstGid);

                std::string tilesArray;
                if (!extractArray(tilesetObj, "tiles", tilesArray)) {
                    continue;
                }

                const auto tiles = splitObjects(tilesArray);
                for (const auto& tileObj : tiles) {
                    int id = 0;
                    if (!extractInt(tileObj, "id", id)) {
                        continue;
                    }
                    const auto kind = getTileKind(tileObj);
                    if (kind != TileKind::None) {
                        gidKinds[firstGid + id] = kind;
                    }
                }
            }
        }

        std::string layersArray;
        if (!extractArray(json, "layers", layersArray)) {
            return;
        }

        const auto layers = splitObjects(layersArray);
        for (const auto& layerObj : layers) {
            std::string name;
            if (!extractString(layerObj, "name", name) || name != "Collision_Layer") {
                continue;
            }

            std::vector<int> dataList;
            extractIntArray(layerObj, "data", dataList);
            if (dataList.empty() || height <= 0) {
                continue;
            }

            const int width = static_cast<int>(dataList.size()) / height;
            if (width <= 0) {
                continue;
            }

            for (int row = 0; row < height; ++row) {
                int col = 0;
                while (col < width) {
                    const int index = row * width + col;
                    if (index < 0 || index >= static_cast<int>(dataList.size())) {
                        break;
                    }
                    const int gid = dataList[index];
                    const auto it = gidKinds.find(gid);
                    if (gid == 0 || it == gidKinds.end()) {
                        ++col;
                        continue;
                    }

                    const TileKind kind = it->second;
                    int startCol = col;
                    int endCol = col + 1;
                    while (endCol < width) {
                        const int nextIndex = row * width + endCol;
                        if (nextIndex < 0 || nextIndex >= static_cast<int>(dataList.size())) {
                            break;
                        }
                        const int nextGid = dataList[nextIndex];
                        const auto nextIt = gidKinds.find(nextGid);
                        if (nextGid == 0 || nextIt == gidKinds.end() || nextIt->second != kind) {
                            break;
                        }
                        ++endCol;
                    }

                    const int runLen = endCol - startCol;
                    rtp::server::ObstacleEvent event{};
                    event.atTime = 0.0f;
                    event.position = {static_cast<float>(startCol * tileWidth),
                                      static_cast<float>(row * tileHeight)};
                    event.size = {static_cast<float>(runLen * tileWidth),
                                  static_cast<float>(tileHeight)};
                    if (kind == TileKind::Destructible) {
                        event.health = 80;
                        event.type = rtp::net::EntityType::Obstacle;
                    } else {
                        event.health = 1000000;
                        event.type = rtp::net::EntityType::ObstacleSolid;
                    }
                    obstacles.push_back(event);

                    col = endCol;
                }
            }
        }
    }
}

namespace rtp::server {

std::optional<LevelData> loadLevelFromFile(const std::string& path, std::string& error)
{
    auto data = readFile(path);
    if (!data) {
        error = "Failed to read level file: " + path;
        return std::nullopt;
    }

    LevelData level;
    const std::string& json = data.value();

    int id = 0;
    if (extractInt(json, "level_id", id)) {
        level.id = static_cast<uint32_t>(id);
    }
    extractString(json, "name", level.name);

    std::string metadataObj;
    if (extractObject(json, "game_metadata", metadataObj)) {
        float width = 0.0f;
        if (extractNumber(metadataObj, "level_width_in_pixels", width)) {
            level.widthPixels = width;
        }
        extractVec2(metadataObj, "player_start_position", level.playerStart);
        extractString(metadataObj, "tileset_path", level.tilesetPath);
    }

    std::string spawnArray;
    if (extractArray(json, "spawn_triggers", spawnArray)) {
        const auto objects = splitObjects(spawnArray);
        for (const auto& obj : objects) {
            float time = 0.0f;
            if (!extractNumber(obj, "at_time", time)) {
                extractNumber(obj, "at_position", time);
            }

            std::string templatePath;
            extractString(obj, "template_path", templatePath);

            std::string typeStr;
            extractString(obj, "entity_type", typeStr);
            const rtp::net::EntityType entityType = parseEntityType(typeStr, templatePath);

            rtp::Vec2f startPos{0.0f, 0.0f};
            extractVec2(obj, "start_position", startPos);

            int count = 1;
            extractInt(obj, "count", count);

            float delay = 0.0f;
            extractNumber(obj, "delay_between_spawn", delay);

            std::string patternStr;
            extractString(obj, "pattern", patternStr);
            const auto pattern = patternStr.empty()
                ? rtp::ecs::components::Patterns::StraightLine
                : parsePattern(patternStr);

            float speed = 120.0f;
            float amplitude = 40.0f;
            float frequency = 2.0f;
            extractNumber(obj, "speed", speed);
            extractNumber(obj, "amplitude", amplitude);
            extractNumber(obj, "frequency", frequency);

            count = std::max(1, count);
            for (int i = 0; i < count; ++i) {
                SpawnEvent spawn{};
                spawn.atTime = time + (delay * static_cast<float>(i));
                spawn.type = entityType;
                spawn.startPosition = startPos;
                spawn.pattern = pattern;
                spawn.speed = speed;
                spawn.amplitude = amplitude;
                spawn.frequency = frequency;
                level.spawns.push_back(spawn);
            }
        }
    }

    std::string powerupArray;
    if (extractArray(json, "powerup_triggers", powerupArray)) {
        const auto objects = splitObjects(powerupArray);
        for (const auto& obj : objects) {
            PowerupEvent event{};
            extractNumber(obj, "at_time", event.atTime);

            std::string typeStr;
            extractString(obj, "type", typeStr);
            event.type = parsePowerupType(typeStr);

            extractVec2(obj, "position", event.position);
            extractNumber(obj, "value", event.value);
            extractNumber(obj, "duration", event.duration);
            level.powerups.push_back(event);
        }
    }

    std::string obstacleArray;
    if (extractArray(json, "obstacle_triggers", obstacleArray)) {
        const auto objects = splitObjects(obstacleArray);
        for (const auto& obj : objects) {
            ObstacleEvent event{};
            extractNumber(obj, "at_time", event.atTime);
            extractVec2(obj, "position", event.position);
            extractVec2WH(obj, "size", event.size);
            extractInt(obj, "health", event.health);
            level.obstacles.push_back(event);
        }
    }

    if (!level.tilesetPath.empty()) {
        appendTileObstacles(level.tilesetPath, level.obstacles);
    }

    auto sortByTime = [](const auto& a, const auto& b) { return a.atTime < b.atTime; };
    std::sort(level.spawns.begin(), level.spawns.end(), sortByTime);
    std::sort(level.powerups.begin(), level.powerups.end(), sortByTime);
    std::sort(level.obstacles.begin(), level.obstacles.end(), sortByTime);

    return level;
}

}  // namespace rtp::server
