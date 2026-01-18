/**
 * File   : Boomerang.hpp
 * License: MIT
 */

#pragma once

#include "RType/Math/Vec2.hpp"

namespace rtp::ecs::components {

/**
 * @struct Boomerang
 * @brief Marks a projectile as a boomerang and stores state for return logic.
 */
struct Boomerang {
    uint32_t ownerIndex{0};    /**< ECS entity index of the owner who fired this boomerang */
    rtp::Vec2f startPos{0.0f, 0.0f}; /**< Spawn position to compute travel distance */
    float maxDistance{400.0f}; /**< Distance before returning */
    bool returning{false};     /**< Whether the boomerang is on its way back */
};

} // namespace rtp::ecs::components
