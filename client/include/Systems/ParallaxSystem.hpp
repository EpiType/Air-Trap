#pragma once

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/ParallaxLayer.hpp"

namespace rtp::client {

class ParallaxSystem : public ecs::ISystem {
    public:
        explicit ParallaxSystem(ecs::Registry& r) : _r(r) {}

        void update(float dt) override {
        auto view = _r.zipView<ecs::components::Transform, ecs::components::ParallaxLayer>();

        for (auto&& [transform, parallax] : view) {
            transform.position.x -= parallax.speed * dt;

            float scaledWidth = parallax.textureWidth * transform.scale.x;
            if (transform.position.x <= -scaledWidth) {
                transform.position.x += scaledWidth * 2.0f;
            }
        }
    }

    private:
        ecs::Registry& _r;
    };
} // namespace rtp::client