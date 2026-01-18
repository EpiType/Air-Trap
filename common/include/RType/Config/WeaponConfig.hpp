/*
** WeaponConfig.hpp
** Header-only loader for weapon definitions (custom JSON parser, no external deps)
*/
#pragma once

#include "RType/ECS/Components/SimpleWeapon.hpp"
#include "RType/Config/SimpleJsonParser.hpp"
#include "RType/Logger.hpp"
#include <unordered_map>
#include <string>

namespace rtp::config {

    inline std::unordered_map<rtp::ecs::components::WeaponKind, rtp::ecs::components::SimpleWeapon> loadWeaponConfigs(const std::string &path = "config/common/weapons.json")
    {
        std::unordered_map<rtp::ecs::components::WeaponKind, rtp::ecs::components::SimpleWeapon> map;

        // Try multiple candidate locations to be robust to working-directory differences
        std::vector<std::string> candidates = {
            path,
            "config/common/weapons.json",
            "config/weapons.json",
            "./config/common/weapons.json",
            "../config/common/weapons.json"
        };

        std::unordered_map<std::string, SimpleJson> weapons;
        for (const auto &p : candidates) {
            weapons = parseWeaponsFile(p);
            if (!weapons.empty()) break;
        }

        if (weapons.empty()) {
            return map; // empty => callers should fallback to hardcoded defaults
        }

        for (const auto& [name, data] : weapons) {
            rtp::ecs::components::WeaponKind kind = rtp::ecs::components::WeaponKind::Classic;
            
            // Determine kind from data or name
            int kindVal = data.getInt("kind", -1);
            if (kindVal >= 0) {
                kind = static_cast<rtp::ecs::components::WeaponKind>(kindVal);
            } else {
                if (name == "Classic") kind = rtp::ecs::components::WeaponKind::Classic;
                else if (name == "Beam") kind = rtp::ecs::components::WeaponKind::Beam;
                // Paddle (Reflector) removed from configs
                else if (name == "Tracker") kind = rtp::ecs::components::WeaponKind::Tracker;
                else if (name == "Boomerang") kind = rtp::ecs::components::WeaponKind::Boomerang;
            }

            rtp::ecs::components::SimpleWeapon cfg;
            cfg.kind = kind;
            cfg.fireRate = data.getFloat("fireRate", 0.0f);
            cfg.lastShotTime = data.getFloat("lastShotTime", 0.0f);
            cfg.damage = data.getInt("damage", 0);
            cfg.ammo = data.getInt("ammo", -1);
            cfg.maxAmmo = data.getInt("maxAmmo", -1);
            cfg.beamDuration = data.getFloat("beamDuration", 0.0f);
            cfg.beamCooldown = data.getFloat("beamCooldown", 0.0f);
            // Reflect feature removed; no longer loading canReflect
            cfg.homing = data.getBool("homing", false);
            cfg.homingSteering = data.getFloat("homingSteering", 3.0f);
            cfg.homingRange = data.getFloat("homingRange", 600.0f);
            cfg.isBoomerang = data.getBool("isBoomerang", false);
            cfg.difficulty = data.getInt("difficulty", 2);

            map[kind] = cfg;
        }

        return map;
    }

    // Internal cache helpers (function-statics so header-only usage is safe)
    inline std::unordered_map<rtp::ecs::components::WeaponKind, rtp::ecs::components::SimpleWeapon> &weapon_cache()
    {
        static std::unordered_map<rtp::ecs::components::WeaponKind, rtp::ecs::components::SimpleWeapon> cache;
        return cache;
    }

    inline bool &weapon_cache_loaded()
    {
        static bool loaded = false;
        return loaded;
    }

    inline void reloadWeaponConfigs()
    {
        weapon_cache() = loadWeaponConfigs();
        weapon_cache_loaded() = true;
        log::info("Reloaded {} weapon configs", weapon_cache().size());
    }

    inline const rtp::ecs::components::SimpleWeapon &getWeaponDef(rtp::ecs::components::WeaponKind kind)
    {
        if (!weapon_cache_loaded()) {
            weapon_cache() = loadWeaponConfigs();
            weapon_cache_loaded() = true;
        }

        auto it = weapon_cache().find(kind);
        static rtp::ecs::components::SimpleWeapon fallback{};
        if (it != weapon_cache().end())
            return it->second;

        return fallback; // empty fallback; callers may set defaults
    }

    inline bool hasWeaponConfigs()
    {
        if (!weapon_cache_loaded()) {
            weapon_cache() = loadWeaponConfigs();
            weapon_cache_loaded() = true;
        }
        return !weapon_cache().empty();
    }

    inline std::string getWeaponDisplayName(rtp::ecs::components::WeaponKind kind)
    {
        // Try candidate paths like loadWeaponConfigs
        std::vector<std::string> candidates = {
            "config/common/weapons.json",
            "config/weapons.json",
            "./config/common/weapons.json",
            "../config/common/weapons.json"
        };

        for (const auto &p : candidates) {
            auto weapons = parseWeaponsFile(p);
            if (weapons.empty()) continue;

            for (const auto& [name, data] : weapons) {
                int k = data.getInt("kind", -1);
                    if (k < 0) {
                    if (name == "Classic") k = 0;
                    else if (name == "Beam") k = 1;
                    else if (name == "Tracker") k = 3;
                    else if (name == "Boomerang") k = 4;
                    }
                
                if (static_cast<uint8_t>(kind) == static_cast<uint8_t>(k)) {
                    std::string displayName = data.getString("displayName", "");
                    if (!displayName.empty()) return displayName;
                    return name;
                }
            }
        }

        return std::string();
    }

} // namespace rtp::config
