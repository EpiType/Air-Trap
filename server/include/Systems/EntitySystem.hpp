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

    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"
    #include "RType/ECS/Components/NetworkId.hpp"
    #include "RType/ECS/Components/EntityType.hpp"

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
            EntitySystem(rtp::ecs::Registry& registry, ServerNetwork& network);

            /**
             * @brief Update movement system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

            /**
             * @brief Create a new player entity in the ECS
             * @return The ID of the created player entity
             */
            uint32_t createPlayerEntity(PlayerPtr player);

            /**
             * @brief Create a new enemy entity in the ECS
             * @return The ID of the created enemy entity
             */
            uint32_t creaetEnemyEntity(const Vec2f& position);

        protected:
            rtp::ecs::Registry& _registry;   /**< Reference to the entity registry */
            ServerNetwork& _network;         /**< Reference to the server network manager */
    };
}

#endif /* !RTYPE_ENTITY_SYSTEM_HPP_ */