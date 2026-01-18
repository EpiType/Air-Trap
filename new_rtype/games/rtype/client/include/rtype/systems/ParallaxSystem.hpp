/**
 * File   : ParallaxSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_CLIENT_PARALLAX_SYSTEM_HPP_
    #define RTYPE_CLIENT_PARALLAX_SYSTEM_HPP_

    /* Engine */
    #include "engine/ecs/ISystem.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ecs/components/Transform.hpp"

    /* R-Type Client */
    #include "rtype/components/ParallaxLayer.hpp"

namespace rtp::client::systems {
    /**
     * @class ParallaxSystem
     * @brief System to handle parallax of background layers.
     */
    class ParallaxSystem : public aer::ecs::ISystem {
        public:
            /**
             * @brief Constructor for ParallaxSystem
             * @param registry Reference to the entity registry
             */
            explicit ParallaxSystem(aer::ecs::Registry& registry)
                : _registry(registry) {}

            /**
             * @brief Update parallax layers based on their speed and elapsed time.
             * @param deltaTime Time elapsed since last update in seconds
             */
            void update(float dt) override;

        private:
            aer::ecs::Registry& _registry;  /**< Reference to the entity registry */
    };
}

#endif /* !RTYPE_CLIENT_PARALLAX_SYSTEM_HPP_ */