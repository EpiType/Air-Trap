/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Damage
*/

#pragma once
#include "RType/ECS/Entity.hpp"

namespace rtp::ecs::components {
/**
 * @struct Damage
 * @brief Component representing damage value.
 */
struct Damage {
    int amount{0}; /**< Amount of damage */
    Entity sourceEntity{NullEntity}; /**< ID of the entity that caused the damage */
};
}  // namespace rtp::ecs::components