/**
 * File   : EnemyShootSystem.hpp
 * License: MIT
 * Author : Elias
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ENEMY_SHOOT_SYSTEM_HPP_
    #define RTYPE_ENEMY_SHOOT_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"

    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/RoomId.hpp"
    #include "RType/ECS/Components/SimpleWeapon.hpp"
    #include "RType/ECS/Components/BoundingBox.hpp"
    #include "RType/ECS/Components/Damage.hpp"

    #include "Systems/RoomSystem.hpp"
    #include "Systems/NetworkSyncSystem.hpp"

namespace rtp::server {
    /**
     * @class EnemyShootSystem
     * @brief Spawns bullets for enemies on the server.
     */
    class EnemyShootSystem : public rtp::ecs::ISystem {
        public:
            /**
             * @brief Constructor for EnemyShootSystem
             * @param registry Reference to the entity registry
             * @param roomSystem Reference to the RoomSystem
             * @param networkSync Reference to the NetworkSyncSystem
             */
            EnemyShootSystem(rtp::ecs::Registry& registry,
                             RoomSystem& roomSystem,
                             NetworkSyncSystem& networkSync);

            /**
             * @brief Update enemy shooting system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            /**
             * @brief Spawn a bullet entity at the given transform and room
             * @param tf Transform of the shooter entity
             * @param roomId RoomId of the shooter entity
             */
            void spawnBullet(const rtp::ecs::components::Transform& tf,
                             const rtp::ecs::components::RoomId& roomId);

        private:
            rtp::ecs::Registry& _registry;      /**< Reference to the entity registry */
            RoomSystem& _roomSystem;            /**< Reference to the RoomSystem */
            NetworkSyncSystem& _networkSync;    /**< Reference to the NetworkSyncSystem */

            float _bulletSpeed = -350.0f;       /**< Speed of the spawned bullets */
            float _spawnOffsetX = -10.0f;       /**< X offset for bullet spawn position */
    };
}

#endif /* !RTYPE_ENEMY_SHOOT_SYSTEM_HPP_ */
