/**
 * File   : AnimationSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/AnimationSystem.hpp"

namespace rtp::client::systems
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    AnimationSystem::AnimationSystem(aer::ecs::Registry& registry)
        : _registry(registry)
    {
    }

    void AnimationSystem::update(float dt) {
        auto view = _registry.zipView<aer::ecs::components::Sprite, aer::ecs::components::Animation>();

        for (auto&& [sprite, animation] : view) {
            animation.elapsedTime += dt;
                
            while (animation.elapsedTime >= animation.frameDuration) {
                animation.elapsedTime -= animation.frameDuration; 
                
                animation.currentFrame = (animation.currentFrame + 1) % animation.totalFrames;
                    
                sprite.rectLeft = animation.frameLeft + animation.currentFrame * animation.frameWidth;
                sprite.rectTop = animation.frameTop;
            }
        }
    }
} // namespace rtp::client::systems
