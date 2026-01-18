/**
 * File   : AnimationSystem.hpp
 * License: MIT
 * Author : Enzo LOBATO
 * COUTINHO <enzo.lobato-coutinho@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef SCORE_SYSTEM_HPP
#define SCORE_SYSTEM_HPP

#include "RType/ECS/Components/Score.hpp"
#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Registry.hpp"

namespace rtp::client
{
    class ScoreSystem : public ecs::ISystem {
        public:
            explicit ScoreSystem(ecs::Registry &r) : _r(r)
            {
            }

            void update(float dt) override
            {
                auto view = _r.view<ecs::components::Score>();

                for (auto &&score : view) {
                    const int increment = static_cast<int>(dt * 100.0f);
                    score.value += (increment > 0) ? increment : 1;
                }
            }

        private:
            ecs::Registry &_r;
    };
}

#endif // SCORE_SYSTEM_HPP
