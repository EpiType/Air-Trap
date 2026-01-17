/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** ShieldSystem - Manages shield visual effects and removal on damage
*/

#pragma once

#include "RType/ECS/Registry.hpp"
#include "RType/ECS/ISystem.hpp"
#include <SFML/Graphics/RenderWindow.hpp>

namespace rtp::client {

/**
 * @brief System that manages shield visual effects
 * 
 * This system is responsible for:
 * - Updating shield animation (pulse effect)
 * - Detecting when shield should be removed (on collision with enemy bullets)
 */
class ShieldSystem : public ecs::ISystem {
public:
    explicit ShieldSystem(ecs::Registry& registry);
    
    /**
     * @brief Update shield visuals and handle collision-based removal
     * @param dt Delta time in seconds
     */
    void update(float dt) override;

private:
    ecs::Registry& _registry;
};

} // namespace rtp::client
