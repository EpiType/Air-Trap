/**
 * File   : Homing.hpp
 * License: MIT
 * Author : GitHub Copilot (GPT-5 mini)
 */

#pragma once

#include "RType/Math/Vec2.hpp"

namespace rtp::ecs::components {

/**
 * @struct Homing
 * @brief Marks an entity as homing and provides tuning parameters.
 */
struct Homing {
    float steering{3.0f};   /**< Steering factor (per second) used to lerp direction toward target */
    float range{600.0f};    /**< Detection range in pixels */
};

} // namespace rtp::ecs::components
