#pragma once

#include "ISystem.hpp"

namespace ecs {

/**
 * @brief Handles movement and basic collision detection.
 */
class PhysicsSystem : public ISystem {
public:
    void update(Registry& registry, float deltaTime) override;

private:
    // Movement logic: position += velocity * deltaTime
    // Basic collision detection can be added here
};

} // namespace ecs
