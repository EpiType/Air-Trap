/**
 * File   : ShieldSystem.hpp
 * License: MIT
 * Author : Angel SEVERAN <angel.severan@epitech.eu>
 * Date   : 17/01/2026
 */

#pragma once

#include "RType/ECS/Registry.hpp"
#include "RType/ECS/ISystem.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

namespace rtp::client {

/**
 * @class ShieldSystem
 * @brief System responsible for managing shield visual effects
 * 
 * This system handles client-side shield visual behavior:
 * - Animates shield pulsing effect (alpha oscillation between 150-255)
 * - Detects AABB collisions between shielded players and enemy bullets
 * - Removes shield visual when collision is detected
 * 
 * Note: Actual damage blocking is handled server-side by the Shield component.
 * This system only manages the visual feedback on the client.
 * 
 * @see rtp::ecs::components::ShieldVisual
 * @see rtp::ecs::components::Shield (server-side)
 */
class ShieldSystem : public ecs::ISystem {
public:
    /**
     * @brief Construct a new Shield System
     * @param registry The ECS registry containing entities and components
     */
    explicit ShieldSystem(ecs::Registry& registry);
    
    /**
     * @brief Update shield animations and handle collision detection
     * @param dt Delta time in seconds since last frame
     * 
     * Updates:
     * 1. ShieldVisual animation time for pulse effect
     * 2. Alpha value using sine wave (150 + sin(time * 3) * 105)
     * 3. Checks AABB collision with enemy bullets
     * 4. Removes shield visual on collision
     */
    void update(float dt) override;

private:
    ecs::Registry& _registry;
};

} // namespace rtp::client
