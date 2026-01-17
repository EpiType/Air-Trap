/**
 * File   : AnimationSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_ANIMATION_SYSTEM_HPP_
    #define RTYPE_CLIENT_ANIMATION_SYSTEM_HPP_

    /* Engine */
    #include "engine/ecs/ISystem.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ecs/components/Animation.hpp"
    #include "engine/ecs/components/Sprite.hpp"

namespace rtp::client::systems
{
    /**
     * @class AnimationSystem
     * @brief System to update sprite animations.
     */
    class AnimationSystem : public engine::ecs::ISystem
    {
        public:
            /**
             * @brief Constructor for AnimationSystem
             * @param registry Reference to the entity registry
             */
            explicit AnimationSystem(engine::ecs::Registry& registry);

            /**
             * @brief Update animation frames based on elapsed time.
             * @param dt Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            engine::ecs::Registry& _registry;  /**< Reference to the entity registry */
    };
}

#endif /* !RTYPE_CLIENT_ANIMATION_SYSTEM_HPP_ */
