/**
 * File   : BulletCleanupSystem.hpp
 * License: MIT
 * Author : Elias
 * Date   : 11/12/2025
 */

#ifndef RTYPE_BULLET_CLEANUP_SYSTEM_HPP_
    #define RTYPE_BULLET_CLEANUP_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"

    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/RoomId.hpp"

    #include "Systems/RoomSystem.hpp"
    #include "Systems/NetworkSyncSystem.hpp"

namespace rtp::server {
    /**
     * @class BulletCleanupSystem
     * @brief Despawns bullets that leave the play area.
     */
    class BulletCleanupSystem : public ecs::ISystem {
        public:
            /**
             * @brief Constructor for BulletCleanupSystem
             * @param registry Reference to the ECS registry
             * @param roomSystem Reference to the RoomSystem
             * @param networkSync Reference to the NetworkSyncSystem
             */
            BulletCleanupSystem(ecs::Registry& registry,
                                RoomSystem& roomSystem,
                                NetworkSyncSystem& networkSync);

            /**
             * @brief Update system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            /**
             * @brief Despawn an entity and notify players in the room
             * @param entity The entity to despawn
             * @param roomId The ID of the room the entity belongs to
             */
            void despawn(const ecs::Entity& entity, uint32_t roomId);

        private:
            ecs::Registry& _registry;      /**< Reference to the ECS registry */
            RoomSystem& _roomSystem;            /**< Reference to the RoomSystem */
            NetworkSyncSystem& _networkSync;    /**< Reference to the NetworkSyncSystem */

            float _minX = -200.0f;              /**< Left boundary for bullet despawn */
            float _maxX = 1800.0f;              /**< Right boundary for bullet despawn */
    };
}

#endif /* !RTYPE_BULLET_CLEANUP_SYSTEM_HPP_ */
