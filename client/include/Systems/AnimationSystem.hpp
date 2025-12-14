/**
 * File   : AnimationSystem.hpp
 * License: MIT
 * Author : Enzo LOBATO COUTINHO <enzo.lobato-coutinho@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef ANIMATION_SYSTEM_HPP
#define ANIMATION_SYSTEM_HPP

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"

#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Animation.hpp"

namespace rtp::client {
    class AnimationSystem : public rtp::ecs::ISystem {
    public:
        explicit AnimationSystem(rtp::ecs::Registry& r) : _r(r) {}

        void update(float dt) override {
            auto view = _r.zipView<rtp::ecs::components::Sprite, rtp::ecs::components::Animation>();

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
    private:
        rtp::ecs::Registry& _r;
    };
}
#endif // ANIMATION_SYSTEM_HPP