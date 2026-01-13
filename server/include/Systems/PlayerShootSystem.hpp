/**
 * File   : PlayerShootSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_PLAYER_SHOOT_SYSTEM_HPP_
    #define RTYPE_PLAYER_SHOOT_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"

    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/RoomId.hpp"
    #include "RType/ECS/Components/SimpleWeapon.hpp"
    #include "RType/ECS/Components/Ammo.hpp"

    #include "Systems/RoomSystem.hpp"
    #include "Systems/NetworkSyncSystem.hpp"

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {
    /**
     * @class PlayerShootSystem
     * @brief Spawns bullets from player input on the server.
     */
    class PlayerShootSystem : public rtp::ecs::ISystem {
        public:
            PlayerShootSystem(rtp::ecs::Registry& registry,
                              RoomSystem& roomSystem,
                              NetworkSyncSystem& networkSync);

            void update(float dt) override;

        private:
            void spawnBullet(const rtp::ecs::components::Transform& tf,
                             const rtp::ecs::components::RoomId& roomId);
            void sendAmmoUpdate(uint32_t netId, const rtp::ecs::components::Ammo& ammo);

        private:
            rtp::ecs::Registry& _registry;
            RoomSystem& _roomSystem;
            NetworkSyncSystem& _networkSync;

            float _bulletSpeed = 500.0f;
            float _spawnOffsetX = 20.0f;
    };
}

#endif /* !RTYPE_PLAYER_SHOOT_SYSTEM_HPP_ */
