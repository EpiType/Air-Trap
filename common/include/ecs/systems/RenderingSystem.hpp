#pragma once

#include "ISystem.hpp"

namespace ecs {

/**
 * @brief Handles rendering entities with sprites.
 * 
 * This system is client-side only and uses SFML for rendering.
 */
class RenderingSystem : public ISystem {
public:
    void update(Registry& registry, float deltaTime) override;

private:
    // Draw entities with PositionComponent and SpriteComponent
};

} // namespace ecs
