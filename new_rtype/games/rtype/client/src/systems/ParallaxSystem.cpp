/**
 * File   : ParallaxSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#include "rtype/systems/ParallaxSystem.hpp"

namespace rtp::client::systems
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    void ParallaxSystem::update(float dt)
    {
        auto view = _registry.zipView<aer::ecs::components::Transform, rtp::ecs::components::ParallaxLayer>();

        for (auto&& [transform, parallax] : view) {
            transform.position.x -= parallax.speed * dt;

            float scaledWidth = parallax.textureWidth * transform.scale.x;
            if (transform.position.x <= -scaledWidth) {
                transform.position.x += scaledWidth * 2.0f;
            }
        }
    }
} // namespace rtp::client::systems