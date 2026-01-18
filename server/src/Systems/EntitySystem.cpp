/**
 * File   : EntitySystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR
 * LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/EntitySystem.hpp"
#include "RType/Config/WeaponConfig.hpp"

namespace rtp::server
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    EntitySystem::EntitySystem(ecs::Registry &registry,
                               ServerNetwork &network,
                               NetworkSyncSystem &networkSync)
        : _registry(registry)
        , _network(network)
        , _networkSync(networkSync)
    {
    }

    void EntitySystem::update(float dt)
    {
        (void)dt;
    }

    ecs::Entity EntitySystem::createPlayerEntity(PlayerPtr player)
    {
        return createPlayerEntity(player, {100.f, 100.f});
    }

    ecs::Entity
        EntitySystem::createPlayerEntity(PlayerPtr player,
                                         const Vec2f &spawnPos)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn player entity: {}",
                            entityRes.error().message());
            throw std::runtime_error(
                std::string("Failed to spawn player entity: ") +
                std::string(entityRes.error().message()));
        }

        ecs::Entity entity = entityRes.value();

        _registry.add<ecs::components::Transform>(
            entity, ecs::components::Transform{
                        {spawnPos.x, spawnPos.y},
                        0.f,
                        {1.f,        1.f       }
        });

        _registry.add<ecs::components::Velocity>(
            entity, ecs::components::Velocity{
                        {0.f, 0.f},
                        0.f
        });

        ecs::components::SimpleWeapon weapon;
        auto weaponKind = player->getWeaponKind();
        weapon.kind = weaponKind;

        // Prefer configuration file values if available, otherwise fallback to hardcoded defaults
        if (rtp::config::hasWeaponConfigs()) {
            weapon = rtp::config::getWeaponDef(weaponKind);
            weapon.kind = weaponKind;
        } else {
            switch (weaponKind) {
                case ecs::components::WeaponKind::Classic:
                    weapon.fireRate = 6.0f;
                    weapon.damage = 10;
                    weapon.ammo = -1;  // Infinite
                    weapon.maxAmmo = -1;
                    break;
                case ecs::components::WeaponKind::Beam:
                    weapon.fireRate = 0.0f;  // Continuous
                    weapon.damage = 4;  // Per tick
                    weapon.beamDuration = 5.0f;
                    weapon.beamCooldown = 5.0f;
                    weapon.ammo = -1;
                    weapon.maxAmmo = -1;
                    break;
                case ecs::components::WeaponKind::Paddle:
                    weapon.fireRate = 0.0f;
                    weapon.damage = 0;
                    weapon.canReflect = true;
                    weapon.ammo = -1;
                    weapon.maxAmmo = -1;
                    break;
                case ecs::components::WeaponKind::Tracker:
                    weapon.fireRate = 2.0f;
                    weapon.damage = 6;
                    weapon.homing = true;
                    weapon.ammo = 50;
                    weapon.maxAmmo = 50;
                    break;
                case ecs::components::WeaponKind::Boomerang:
                    weapon.fireRate = 0.5f;  // Slow single throw
                    weapon.damage = 18;
                    weapon.isBoomerang = true;
                    weapon.ammo = -1;
                    weapon.maxAmmo = -1;
                    break;
            }
        }

        _registry.add<ecs::components::SimpleWeapon>(entity, weapon);

        _registry.add<ecs::components::Ammo>(
            entity,
            ecs::components::Ammo{100, 100, 2.0f, 0.0f, false, true});

        _registry.add<ecs::components::MovementSpeed>(
            entity, ecs::components::MovementSpeed{200.0f, 1.0f, 0.0f});

        _registry.add<ecs::components::NetworkId>(
            entity, ecs::components::NetworkId{(uint32_t)entity});

        _registry.add<ecs::components::server::InputComponent>(
            entity, ecs::components::server::InputComponent{});

        _registry.add<ecs::components::EntityType>(
            entity,
            ecs::components::EntityType{net::EntityType::Player});

        _registry.add<ecs::components::Health>(
            entity, ecs::components::Health{100, 100});

        _registry.add<ecs::components::BoundingBox>(
            entity, ecs::components::BoundingBox{32.0f, 16.0f});

        _registry.add<ecs::components::RoomId>(
            entity, ecs::components::RoomId{player->getRoomId()});

        return entity;
    }

    void EntitySystem::applyWeaponToEntity(ecs::Entity entity, ecs::components::WeaponKind weaponKind)
    {
        ecs::components::SimpleWeapon wcfg;
        wcfg.kind = weaponKind;

        if (rtp::config::hasWeaponConfigs()) {
            wcfg = rtp::config::getWeaponDef(weaponKind);
            wcfg.kind = weaponKind;
        } else {
            switch (weaponKind) {
                case ecs::components::WeaponKind::Classic:
                    wcfg.fireRate = 6.0f; wcfg.damage = 10; wcfg.ammo = -1; wcfg.maxAmmo = -1; break;
                case ecs::components::WeaponKind::Beam:
                    wcfg.fireRate = 0.0f; wcfg.damage = 4; wcfg.beamDuration = 5.0f; wcfg.beamCooldown = 5.0f; wcfg.ammo = -1; wcfg.maxAmmo = -1; break;
                case ecs::components::WeaponKind::Paddle:
                    wcfg.fireRate = 0.0f; wcfg.damage = 0; wcfg.canReflect = true; wcfg.ammo = -1; wcfg.maxAmmo = -1; break;
                case ecs::components::WeaponKind::Tracker:
                    wcfg.fireRate = 2.0f; wcfg.damage = 6; wcfg.homing = true; wcfg.ammo = 50; wcfg.maxAmmo = 50; break;
                case ecs::components::WeaponKind::Boomerang:
                    wcfg.fireRate = 0.5f; wcfg.damage = 18; wcfg.isBoomerang = true; wcfg.ammo = -1; wcfg.maxAmmo = -1; break;
            }
        }

        auto weaponRes = _registry.get<ecs::components::SimpleWeapon>();
        if (weaponRes) {
            auto &weapons = weaponRes->get();
            if (weapons.has(entity)) {
                weapons[entity] = wcfg;
            } else {
                _registry.add<ecs::components::SimpleWeapon>(entity, wcfg);
            }
        } else {
            _registry.add<ecs::components::SimpleWeapon>(entity, wcfg);
        }

        // Update Ammo component defaults if present
        auto ammoRes = _registry.get<ecs::components::Ammo>();
        if (ammoRes) {
            auto &ammos = ammoRes->get();
            if (ammos.has(entity) && wcfg.maxAmmo >= 0) {
                ammos[entity].max = static_cast<uint16_t>(wcfg.maxAmmo);
                ammos[entity].current = static_cast<uint16_t>(wcfg.ammo > 0 ? wcfg.ammo : ammos[entity].current);
            }
        }

        log::info("Applied weapon {} to entity {}", static_cast<int>(weaponKind), entity.index());
    }

    ecs::Entity EntitySystem::createEnemyEntity(
        uint32_t roomId, const Vec2f &pos,
        ecs::components::Patterns pattern, float speed, float amplitude,
        float frequency, net::EntityType type)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn enemy entity: {}",
                            entityRes.error().message());
            throw std::runtime_error(
                std::string("Failed to spawn enemy entity: ") +
                std::string(entityRes.error().message()));
        }

        ecs::Entity entity = entityRes.value();

        _registry.add<ecs::components::Transform>(
            entity, ecs::components::Transform{
                        {pos.x, pos.y},
                        0.f,
                        {1.f,   1.f  }
        });

        _registry.add<ecs::components::NetworkId>(
            entity, ecs::components::NetworkId{
                        static_cast<uint32_t>(entity.index())});

        _registry.add<ecs::components::EntityType>(
            entity, ecs::components::EntityType{type});

        _registry.add<ecs::components::RoomId>(
            entity, ecs::components::RoomId{roomId});

        int maxHealth = 40;
        if (type == net::EntityType::Tank) {
            maxHealth = 80;
        } else if (type == net::EntityType::Boss) {
            maxHealth = 200;
        }
        _registry.add<ecs::components::Health>(
            entity, ecs::components::Health{maxHealth, maxHealth});

        _registry.add<ecs::components::Velocity>(
            entity, ecs::components::Velocity{
                        {0.f, 0.f},
                        0.f
        });

        _registry.add<ecs::components::MouvementPattern>(
            entity, ecs::components::MouvementPattern{
                        pattern, speed, amplitude, frequency});

        float fireRate = 0.6f;
        if (type == net::EntityType::Tank) {
            fireRate = 0.4f;
        } else if (type == net::EntityType::Boss) {
            fireRate = 1.2f;
        }
        ecs::components::SimpleWeapon enemyWeapon;
        enemyWeapon.kind = ecs::components::WeaponKind::Classic;
        enemyWeapon.fireRate = fireRate;
        enemyWeapon.lastShotTime = 0.0f;
        enemyWeapon.damage = 0;
        _registry.add<ecs::components::SimpleWeapon>(entity, enemyWeapon);

        _registry.add<ecs::components::BoundingBox>(
            entity, ecs::components::BoundingBox{30.0f, 18.0f});

        // _registry.add<ecs::components::IABehaviorComponent>(
        //     entity,
        //     ecs::components::IABehaviorComponent{
        //     ecs::components::IABehavior::Passive, 500.0 }
        // );

        return entity;
    }

    ecs::Entity
        EntitySystem::creaetEnemyEntity(uint32_t roomId, const Vec2f &pos,
                                        ecs::components::Patterns pattern,
                                        float speed, float amplitude,
                                        float frequency)
    {
        return createEnemyEntity(roomId, pos, pattern, speed, amplitude,
                                 frequency, net::EntityType::Scout);
    }

    ecs::Entity EntitySystem::createPowerupEntity(
        uint32_t roomId, const Vec2f &pos,
        ecs::components::PowerupType type, float value, float duration)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn powerup entity: {}",
                            entityRes.error().message());
            throw std::runtime_error(
                std::string("Failed to spawn powerup entity: ") +
                std::string(entityRes.error().message()));
        }

        ecs::Entity entity = entityRes.value();

        _registry.add<ecs::components::Transform>(
            entity, ecs::components::Transform{
                        {pos.x, pos.y},
                        0.f,
                        {1.f,   1.f  }
        });

        const net::EntityType netType =
            (type == ecs::components::PowerupType::Speed) ?
                net::EntityType::PowerupSpeed :
                net::EntityType::PowerupHeal;

        _registry.add<ecs::components::EntityType>(
            entity, ecs::components::EntityType{netType});

        _registry.add<ecs::components::Powerup>(
            entity, ecs::components::Powerup{type, value, duration});

        _registry.add<ecs::components::BoundingBox>(
            entity, ecs::components::BoundingBox{16.0f, 16.0f});

        _registry.add<ecs::components::NetworkId>(
            entity, ecs::components::NetworkId{
                        static_cast<uint32_t>(entity.index())});

        _registry.add<ecs::components::RoomId>(
            entity, ecs::components::RoomId{roomId});

        return entity;
    }

    ecs::Entity EntitySystem::createObstacleEntity(
        uint32_t roomId, const Vec2f &pos, const Vec2f &size,
        int health, net::EntityType type)
    {
        auto entityRes = _registry.spawn();
        if (!entityRes) {
            log::error("Failed to spawn obstacle entity: {}",
                            entityRes.error().message());
            throw std::runtime_error(
                std::string("Failed to spawn obstacle entity: ") +
                std::string(entityRes.error().message()));
        }

        ecs::Entity entity = entityRes.value();

        _registry.add<ecs::components::Transform>(
            entity, ecs::components::Transform{
                        {pos.x, pos.y},
                        0.f,
                        {1.f,   1.f  }
        });

        _registry.add<ecs::components::EntityType>(
            entity, ecs::components::EntityType{type});

        _registry.add<ecs::components::Health>(
            entity, ecs::components::Health{health, health});

        _registry.add<ecs::components::BoundingBox>(
            entity, ecs::components::BoundingBox{size.x, size.y});

        _registry.add<ecs::components::NetworkId>(
            entity, ecs::components::NetworkId{
                        static_cast<uint32_t>(entity.index())});

        _registry.add<ecs::components::RoomId>(
            entity, ecs::components::RoomId{roomId});

        return entity;
    }
} // namespace rtp::server
