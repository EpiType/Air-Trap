/**
 * File   : EnemyAISystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ENEMY_AI_SYSTEM_HPP_
    #define RTYPE_ENEMY_AI_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/ECS/Components/MouvementPattern.hpp"
    #include "RType/ECS/Components/RoomId.hpp"

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {
    /**
     * @class EnemyAISystem
     * @brief System to handle enemy AI behavior.
     */
    class EnemyAISystem : public rtp::ecs::ISystem {
        public:
            /**
             * @brief Constructor for EnemyAISystem
             * @param registry Reference to the entity registry
             */
            EnemyAISystem(rtp::ecs::Registry& registry);

            /**
             * @brief Update enemy AI system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;
        private:
            rtp::ecs::Registry& _registry;   /**< Reference to the entity registry */
            float _time{0.0f};               /**< Elapsed time for AI calculations */
    };
}
#endif /* !RTYPE_ENEMY_AI_SYSTEM_HPP_ */
