/**
 * File   : MovementSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_MOVEMENT_SYSTEM_HPP_
    #define RTYPE_MOVEMENT_SYSTEM_HPP_

    #include "RType/ECS/ISystem.hpp"
    #include "RType/ECS/Registry.hpp"
    
    #include "RType/ECS/Components/InputComponent.hpp"
    #include "RType/ECS/Components/Transform.hpp"

/**
 * @namespace rtp::server
 * @brief Systems for R-Type server
 */
namespace rtp::server {
    /**
     * @class MovementSystem
     * @brief System to handle entity movement based on input components.
     */
    class MovementSystem : public rtp::ecs::ISystem {
        public:
            /**
             * @brief Constructor for MovementSystem
             * @param registry Reference to the entity registry
             */
            MovementSystem(rtp::ecs::Registry& registry);
            
            /**
             * @brief Update movement system logic for one frame
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            rtp::ecs::Registry& _registry;   /**< Reference to the entity registry */
    };
}

#endif /* !RTYPE_MOVEMENT_SYSTEM_HPP_ */