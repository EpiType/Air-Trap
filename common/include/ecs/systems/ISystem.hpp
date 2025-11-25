#pragma once

namespace ecs {

class Registry;

/**
 * @brief Base interface for all systems.
 * 
 * Systems operate on entities with specific components.
 */
class ISystem {
public:
    virtual ~ISystem() = default;

    /**
     * @brief Update the system logic.
     * @param registry The ECS registry
     * @param deltaTime Time elapsed since last update (in seconds)
     */
    virtual void update(Registry& registry, float deltaTime) = 0;
};

} // namespace ecs
