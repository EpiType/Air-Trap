/**
 * File   : PlayerMouvementSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_PLAYER_MOUVEMENT_SYSTEM_HPP_
    #define RTYPE_PLAYER_MOUVEMENT_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    
    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Velocity.hpp"
    #include "RType/ECS/Components/EntityType.hpp"
    #include "RType/ECS/Components/MovementSpeed.hpp"

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {
    /**
     * @class PlayerMouvementSystem
     * @brief System to handle player entity movement based on input components.
     */
    class PlayerMouvementSystem : public ecs::ISystem {
        public:
            /**
             * @brief Constructor for PlayerMouvementSystem
             * @param registry Reference to the entity registry
             */
            PlayerMouvementSystem(ecs::Registry& registry);

            /**
             * @brief Update player movement system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            ecs::Registry& _registry;   /**< Reference to the entity registry */
    };
}
#endif /* !RTYPE_PLAYER_MOUVEMENT_SYSTEM_HPP_ */
