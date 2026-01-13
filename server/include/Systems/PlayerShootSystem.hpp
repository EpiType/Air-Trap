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
    #include "RType/ECS/Components/BoundingBox.hpp"
    #include "RType/ECS/Components/Damage.hpp"

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
            /**
             * @brief Constructor for PlayerShootSystem
             * @param registry Reference to the entity registry
             * @param roomSystem Reference to the RoomSystem
             * @param networkSync Reference to the NetworkSyncSystem
             */
            PlayerShootSystem(rtp::ecs::Registry& registry,
                              RoomSystem& roomSystem,
                              NetworkSyncSystem& networkSync);

            /**
             * @brief Update player shoot system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            /**
             * @brief Spawn a bullet entity based on the player's transform and room ID
             * @param tf Transform component of the player
             * @param roomId RoomId component of the player
             */
            void spawnBullet(const rtp::ecs::components::Transform& tf,
                             const rtp::ecs::components::RoomId& roomId);

            /**
             * @brief Spawn a charged bullet based on the player's transform and room ID
             * @param tf Transform component of the player
             * @param roomId RoomId component of the player
             * @param chargeRatio Charge ratio in [0, 1]
             */
            void spawnChargedBullet(const rtp::ecs::components::Transform& tf,
                                    const rtp::ecs::components::RoomId& roomId,
                                    float chargeRatio);

            /**
             * @brief Send an ammo update to the client for a specific network ID
             * @param netId Network ID of the entity
             * @param ammo Ammo component to send
             */
            void sendAmmoUpdate(uint32_t netId, const rtp::ecs::components::Ammo& ammo);

        private:
            rtp::ecs::Registry& _registry;      /**< Reference to the entity registry */
            RoomSystem& _roomSystem;            /**< Reference to the RoomSystem */
            NetworkSyncSystem& _networkSync;    /**< Reference to the NetworkSyncSystem */

            float _bulletSpeed = 500.0f;        /**< Speed of the spawned bullets */
            float _chargedBulletSpeed = 280.0f; /**< Speed of the charged bullets */
            float _spawnOffsetX = 20.0f;        /**< X offset for bullet spawn position */
    };
}

#endif /* !RTYPE_PLAYER_SHOOT_SYSTEM_HPP_ */
