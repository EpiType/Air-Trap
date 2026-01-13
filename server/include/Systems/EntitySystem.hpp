/**
 * File   : EntitySystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ENTITY_SYSTEM_HPP_
    #define RTYPE_ENTITY_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "Game/Player.hpp"
    #include "ServerNetwork/ServerNetwork.hpp"
    #include "RType/Network/Packet.hpp"
    #include "Systems/NetworkSyncSystem.hpp"

    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/Health.hpp"
    #include "RType/ECS/Components/RoomId.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/ECS/Components/MouvementPattern.hpp"
    #include "RType/ECS/Components/IABehavior.hpp"
    #include "RType/ECS/Components/RoomId.hpp"
    #include "RType/ECS/Components/SimpleWeapon.hpp"
    #include "RType/ECS/Components/Ammo.hpp"
    #include "RType/ECS/Components/BoundingBox.hpp"
    #include "RType/ECS/Components/Damage.hpp"
    #include "RType/ECS/Components/Powerup.hpp"
    #include "RType/ECS/Components/MovementSpeed.hpp"

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {
    /**
     * @class EntitySystem
     * @brief Base class for systems that operate on entities in the ECS.
     */
    class EntitySystem : public rtp::ecs::ISystem {
        public:
            /**
             * @brief Constructor for EntitySystem
             * @param registry Reference to the entity registry
             */
            EntitySystem(rtp::ecs::Registry& registry, ServerNetwork& network, NetworkSyncSystem& networkSync);

            /**
             * @brief Update movement system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

            /**
             * @brief Create a new player entity in the ECS
             * @return The ID of the created player entity
             */
            rtp::ecs::Entity createPlayerEntity(PlayerPtr player);
            rtp::ecs::Entity createPlayerEntity(PlayerPtr player, const rtp::Vec2f& spawnPos);

            /**
             * @brief Create a new enemy entity in the ECS
             * @return The ID of the created enemy entity
             */
            rtp::ecs::Entity createEnemyEntity(
                uint32_t roomId,
                const rtp::Vec2f& pos,
                rtp::ecs::components::Patterns pattern,
                float speed,
                float amplitude,
                float frequency,
                rtp::net::EntityType type = rtp::net::EntityType::Scout
            );

            /**
             * @brief Create a new enemy entity in the ECS with default type Scout
             * @param roomId ID of the room the enemy belongs to
             * @param pos Position to spawn the enemy at
             * @param pattern Movement pattern for the enemy
             * @param speed Movement speed of the enemy
             * @param amplitude Amplitude for movement patterns that require it
             * @param frequency Frequency for movement patterns that require it
             * @return The ID of the created enemy entity
             */
            rtp::ecs::Entity creaetEnemyEntity(
                uint32_t roomId,
                const rtp::Vec2f& pos,
                rtp::ecs::components::Patterns pattern,
                float speed,
                float amplitude,
                float frequency
            );

            /**
             * @brief Create a new powerup entity in the ECS
             * @param roomId ID of the room the powerup belongs to
             * @param pos Position to spawn the powerup at
             * @param type Type of the powerup
             * @param value Value associated with the powerup
             * @param duration Duration the powerup effect lasts
             * @return The ID of the created powerup entity
             */
            rtp::ecs::Entity createPowerupEntity(
                uint32_t roomId,
                const rtp::Vec2f& pos,
                rtp::ecs::components::PowerupType type,
                float value,
                float duration
            );

            /**
             * @brief Create a new obstacle entity in the ECS
             * @param roomId ID of the room the obstacle belongs to
             * @param pos Position to spawn the obstacle at
             * @param size Size of the obstacle
             * @param health Health points of the obstacle
             * @param type Type of the obstacle entity
             * @return The ID of the created obstacle entity
             */
            rtp::ecs::Entity createObstacleEntity(
                uint32_t roomId,
                const rtp::Vec2f& pos,
                const rtp::Vec2f& size,
                int health,
                rtp::net::EntityType type = rtp::net::EntityType::Obstacle
            );

        protected:
            rtp::ecs::Registry& _registry;   /**< Reference to the entity registry */
            ServerNetwork& _network;         /**< Reference to the server network manager */
            NetworkSyncSystem& _networkSync; /**< Reference to the network sync system */
    };
}

#endif /* !RTYPE_ENTITY_SYSTEM_HPP_ */
