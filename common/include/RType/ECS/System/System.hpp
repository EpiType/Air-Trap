/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** System
*/

#pragma once
#include <set>

#include "RType/ECS/Types.hpp"

namespace rtp::ecs {
/**
 * @class System
 * @brief Base class for all systems in the ECS architecture.
 *
 * Systems are responsible for processing entities that possess specific
 * component signatures. This class serves as a base for derived systems
 * that implement specific game logic.
 */
class System {
   public:
    std::set<Entity> _Entities; /**< Set of entities managed by the system */
};
}  // namespace rtp::ecs