/**
 * File   : AnimationSystem.hpp
 * License: MIT
 * Author : Enzo LOBATO COUTINHO <enzo.lobato-coutinho@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef SCORE_SYSTEM_HPP
#define SCORE_SYSTEM_HPP

#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"

#include "RType/ECS/Components/Score.hpp"

namespace rtp::client {
    class ScoreSystem : public rtp::ecs::ISystem {
    public:
        explicit ScoreSystem(rtp::ecs::Registry& r) : _r(r) {}

        void update(float dt) override {
            auto view = _r.view<rtp::ecs::components::Score>();

            for (auto&& score : view) {
                score.value += 10;
            }
        }
    private:
        rtp::ecs::Registry& _r;
    };
}

#endif // SCORE_SYSTEM_HPP