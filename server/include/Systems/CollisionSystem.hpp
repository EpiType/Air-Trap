/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** CollisionSystem
*/

#pragma once

#include <unordered_set>

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/BoundingBox.hpp"
#include "RType/ECS/Components/EntityType.hpp"
#include "RType/ECS/Components/RoomId.hpp"
#include "RType/ECS/Components/NetworkId.hpp"
#include "RType/ECS/Components/Health.hpp"
#include "RType/ECS/Components/Damage.hpp"
#include "RType/ECS/Components/Powerup.hpp"
#include "RType/ECS/Components/MovementSpeed.hpp"
#include "RType/ECS/Components/Velocity.hpp"

#include "Systems/RoomSystem.hpp"
#include "Systems/NetworkSyncSystem.hpp"

namespace rtp::server {

class CollisionSystem : public rtp::ecs::ISystem {
    public:
        /**
         * @brief Constructor for CollisionSystem
         * @param registry Reference to the ECS registry
         * @param roomSystem Reference to the RoomSystem
         * @param networkSync Reference to the NetworkSyncSystem
         */
        CollisionSystem(rtp::ecs::Registry& registry,
                        RoomSystem& roomSystem,
                        NetworkSyncSystem& networkSync);

        /**
         * @brief Update system logic for one frame
         * @param dt Time elapsed since last update in seconds
         */
        void update(float dt) override;

    private:
        /**
         * @brief Check if two entities overlap based on their transforms and bounding boxes
         * @param a Transform of the first entity
         * @param ab BoundingBox of the first entity
         * @param b Transform of the second entity
         * @param bb BoundingBox of the second entity
         * @return true if the entities overlap, false otherwise
         */
        bool overlaps(const rtp::ecs::components::Transform& a,
                      const rtp::ecs::components::BoundingBox& ab,
                      const rtp::ecs::components::Transform& b,
                      const rtp::ecs::components::BoundingBox& bb) const;

        /**
         * @brief Handle collision between two entities
         * @param entityA First entity involved in the collision
         * @param entityB Second entity involved in the collision
         */
        void despawn(const rtp::ecs::Entity& entity, uint32_t roomId);
    
    private:
        rtp::ecs::Registry& _registry;      /**< Reference to the ECS registry */
        RoomSystem& _roomSystem;            /**< Reference to the RoomSystem */
        NetworkSyncSystem& _networkSync;    /**< Reference to the NetworkSyncSystem */
};

}  // namespace rtp::server
